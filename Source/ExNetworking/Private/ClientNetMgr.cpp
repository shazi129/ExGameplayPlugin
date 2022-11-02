#include "ClientNetMgr.h"
#include "ExNetWorkingModule.h"
#include "ClientNetUtil.h"

UClientNetMgr::UClientNetMgr(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		WaitRecvTime = 3.0f;
		RecivedMsgID = WaitingMsgID = -1;
	}
	_currentRecvBuff.clear();
}


UClientNetMgr::~UClientNetMgr()
{
	EXNETWORKING_LOG(Warning, TEXT("---UClientNetMgr Deconstruct"));
}

void UClientNetMgr::InitConnection(FString URL)
{
	if (URL.IsEmpty())
	{
		return;
	}

	_connectioURL = URL;

	TArray<FString> ResultList;
	URL.ParseIntoArray(ResultList, TEXT(":"));
	checkf(ResultList.Num() == 2, TEXT("the url format error: %s, we need format like 127.0.0.1:123"), *URL);

	_host = ResultList[0];
	_port = FCString::Atoi(*ResultList[1]);

	OnAppResumedDelegate = FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UClientNetMgr::HandleAppResumed);
}

void UClientNetMgr::HandleAppResumed()
{
	EXNETWORKING_LOG(Warning, TEXT("---UClientNetMgr::HandleAppResumed"));
}

bool UClientNetMgr::CheckIfHeartTimeout()
{
	if (m_pSocket && m_pSocket.IsValid())
	{
		if (m_pSocket->GetState() == Connected && LastActiveTime > 0)
		{
			if (FDateTime::Now().ToUnixTimestamp() - LastActiveTime > HeartBeatTimeout)
			{
				LastActiveTime = 0;
				return true;
			}
		}
	}
	return false;
}

void UClientNetMgr::Tick(float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(UClientNetMgr_Tick);

	if (m_pSocket && m_pSocket.IsValid())
	{
		if (m_pSocket->ProcError())
		{
			// 处理到异常后,此次tick不再做后续操作
			return;
		}

		m_pSocket->Update();

		if (CheckIfHeartTimeout())//|| bAppResumedReconnect
		{
			bAppResumedReconnect = false;
			EXNETWORKING_LOG(Warning, TEXT("---UClientNetMgr Heart beat time out"));
			ErrorNotifyDelegate.Broadcast("Heart beat time out");
		}

		if (m_pSocket)
		{
			//update中更新socket信息，会导致socket可能被释放，再GetState可能导致崩溃
			if (SendHeartBeatCallBack && m_pSocket->GetState() == Connected)
			{
				if (FDateTime::Now().ToUnixTimestamp() - LastHeartBeatTime > HeartBeatInterval)
				{
					SendHeartBeatCallBack();
					//UE_LOG(LogClient, Warning, TEXT("Send Heart..."));
					LastHeartBeatTime = FDateTime::Now().ToUnixTimestamp();
				}
			}
		}
	}

	if (WaitingMsgID != -1)
	{
		if (WaitingMsgID != RecivedMsgID)
		{
			WaitingTime += DeltaTime;
		}
		else
		{
			WaitingTime = 0;
			WaitingMsgID = -1;
			RecivedMsgID = -1;
		}

		if (WaitingTime > WaitRecvTime)
		{
			// 			if (FClientNetDelegates::OnClientNetErrorNotify.IsBound())
			// 			{
			// 				FClientNetDelegates::OnClientNetErrorNotify.Broadcast((LOCTEXT("ReciveMsgTimeOut", "接受协议超时 Recieve Msg Time Out!").ToString()));
			// 			}
			WaitingTime = 0;
			WaitingMsgID = -1;
			RecivedMsgID = -1;
		}
	}
	else
	{
		WaitingTime = 0;
	}
}

void UClientNetMgr::Shutdown()
{
	if (m_pSocket && m_pSocket.IsValid())
	{
		m_pSocket->Close();
	}

	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.Remove(OnAppResumedDelegate);
}

void UClientNetMgr::OnConnected(bool bSuccessful, int ErrorCode)
{
	if (bSuccessful)
	{
		LastActiveTime = FDateTime::Now().ToUnixTimestamp();
	}
	ConnectedNotifyDelegate.Broadcast(bSuccessful, ErrorCode);

	BP_OnConnected(bSuccessful, ErrorCode);
}

void UClientNetMgr::BP_OnConnected_Implementation(bool bSuccessful, int ErrorCode)
{
}

void UClientNetMgr::OnReceiveData(const char* Data, size_t Size)
{
	LastActiveTime = FDateTime::Now().ToUnixTimestamp();

	//长度不够
	_currentRecvBuff.append(Data, Size);
	int currentRecvDataSize = _currentRecvBuff.size() - 4;
	if (currentRecvDataSize <= 0)
	{
		return;
	}

	while (true)
	{
		uint32 MsgSize = 0;
		FMemory::Memcpy(&MsgSize, _currentRecvBuff.c_str(), 4);
		MsgSize = FClientNetUtil::t_ntohl(MsgSize);
		if ((int)MsgSize > currentRecvDataSize)
		{
			break;
		}

		//移除数据头部
		_currentRecvBuff.erase(0, 4);

		//通知读取
		TArray<uint8> arrayData((const uint8*)_currentRecvBuff.c_str(), (uint32)MsgSize);
		ReceivedDataNotifyDelegate.Broadcast(arrayData);
		BP_OnReceiveData(arrayData);

		//移除真实数据
		_currentRecvBuff.erase(0, MsgSize);
	}
}

void UClientNetMgr::BP_OnReceiveData_Implementation(UPARAM(ref) TArray<uint8>& data)
{

}

void UClientNetMgr::Connect(int32 Timeout)
{
	// 只会新建一个FAsyncSocket
	if (!m_pSocket)
	{
		m_pSocket.Reset(new FAsyncSocket());
	}
	m_pSocket->InitAsyncSocket();

	//连接成功回调
	TFunction<void(bool, int)> ConnecteCallBack = [this](bool bSuccessful, int ErrorCode)
	{
		OnConnected(bSuccessful, ErrorCode);
	};
	m_pSocket->SetConnecteCallBack(ConnecteCallBack);

	//接收数据回调
	TFunction<void(const char*, size_t)> RecvCallBack = [this](const char* Data, size_t Size)
	{
		OnReceiveData(Data, Size);
	};
	m_pSocket->SetRecvCallBack(RecvCallBack);

	//出错回调
	TFunction<void(const TCHAR*)> ErrorCallBack = [&](const TCHAR* Error)
	{
		ErrorNotifyDelegate.Broadcast(Error);
	};
	m_pSocket->SetErrorCallBack(ErrorCallBack);

	LastActiveTime = 0;

	// 建立链接
	m_pSocket->Connect(_host, _port, Timeout);
}

void UClientNetMgr::Disconnect()
{
	if (m_pSocket && m_pSocket.IsValid())
	{
		m_pSocket->Close();
	}
}

void UClientNetMgr::Send(const uint8* Data, uint32 Len)
{
	char buffer[4];
	int NetSize = FClientNetUtil::t_htonl(Len);
	FMemory::Memcpy(&buffer[0], (void*)&NetSize, 4);

	std::string sSendData;
	sSendData.append(&buffer[0], 4);
	sSendData.append((const char*)Data, Len);

	if (m_pSocket && m_pSocket.IsValid() && m_pSocket->GetState() == ESocketState::Connected)
	{
		m_pSocket->Send(sSendData.c_str(), sSendData.size());
	}
	else
	{
		EXNETWORKING_LOG(Error, TEXT("m_pSocket not valid"));
	}
}

void UClientNetMgr::BP_Send_Implementation(const FString& msg)
{
	Send((uint8*)TCHAR_TO_UTF8(*msg), msg.Len());
}

void UClientNetMgr::BP_SendArray_Implementation(const TArray<uint8>& msg)
{
	Send(msg.GetData(), msg.Num());
}

