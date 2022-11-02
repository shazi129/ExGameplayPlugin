#include "AsyncSocket.h"
#include "Misc/ScopeRWLock.h"
#include "ExNetworkingModule.h"
#include <functional>
#include "ClientNetUtil.h"

FAsyncSocket::FAsyncSocket()
{
	// 创建网络线程
	_AccessShared.m_bShutdown = false;
	auto func = std::bind(&FAsyncSocket::SocketRun, this);
	_MainAccessOnly.m_thread = std::thread(func);
	_MainAccessOnly.bFirstReset = true;
	EXNETWORKING_LOG(Log, TEXT("FAsyncSocket(). this:%p"), this);
}

FAsyncSocket::~FAsyncSocket()
{
	EXNETWORKING_LOG(Log, TEXT("~FAsyncSocket(). this:%p"), this);
	// 回收网络线程
	_AccessShared.m_bShutdown = true;
	_MainAccessOnly.m_thread.join();
}

void FAsyncSocket::InitAsyncSocket()
{
	// 初始化
	//_AccessShared.m_Main2SocketTaskQueue.Empty();//用法错误，线程不安全，注掉
	_AccessShared.m_Socket2MainTaskQueue.Empty();
	_AccessShared.m_Socket2MainErrorTaskQueue.Empty();
	_AccessShared.State = ESocketState::Disconnected;

	if (!_MainAccessOnly.bFirstReset)
	{
		auto destoryTask = [this]() {
			EXNETWORKING_LOG(Log, TEXT("SocketThreadTask: destoryTask exec.Host:%s,Port:%d. this:%p"),
				*_SocketAccessOnly.AddrHost, _SocketAccessOnly.AddrPort, this);
#if PLATFORM_SWITCH
			FRWScopeLock Lock(SocketNifmLock, SLT_ReadOnly);
#endif
			_SocketAccessOnly.Socket->Close();
			_SocketAccessOnly.SocketSubsystem->DestroySocket(_SocketAccessOnly.Socket);
			_SocketAccessOnly.Socket = nullptr;
		};

		_AccessShared.m_Main2SocketTaskQueue.Enqueue(destoryTask);
	}

	auto initTask = [this]() {
		EXNETWORKING_LOG(Log, TEXT("SocketThreadTask: initSocket exec. this:%p"), this);

		_SocketAccessOnly.ResolveInfo = nullptr;
		_SocketAccessOnly.RecvBuff.clear();
		_SocketAccessOnly.SendBuff.clear();
		_SocketAccessOnly.Timeout = 0;
		_SocketAccessOnly.StartTime = 0;

#if PLATFORM_SWITCH
		FPlatformMisc::AllowNifm();
#endif
		_SocketAccessOnly.SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

		_SocketAccessOnly.Socket = _SocketAccessOnly.SocketSubsystem->CreateSocket(NAME_Stream, TEXT("client"), false);
		_SocketAccessOnly.Socket->SetNonBlocking();
	};
	_MainAccessOnly.bFirstReset = false;
	_AccessShared.m_Main2SocketTaskQueue.Enqueue(initTask);
}

void FAsyncSocket::Connect(const FString& Host, uint16 Port, int Timeout)
{
	auto connectTask = [=]() {
		EXNETWORKING_LOG(Log, TEXT("SocketThreadTask: connectTask exec,Host:%s,Port:%d. This:%p"), *Host, Port, this);

		TSharedPtr<FInternetAddr> Addr = _SocketAccessOnly.SocketSubsystem->CreateInternetAddr();
		bool IsValid = false;
		_SocketAccessOnly.AddrHost = Host;
		_SocketAccessOnly.AddrPort = Port;
		Addr->SetIp(*Host, IsValid);
		Addr->SetPort(Port);
#if PLATFORM_SWITCH
		FRWScopeLock Lock(SocketNifmLock, SLT_ReadOnly);
#endif
		if (IsValid)
		{
			// valid ip, connect directly
			ConnectByIp(*Addr, Timeout);
		}
		else
		{
			// async dsn resolution
			_AccessShared.State = ESocketState::DNSResolving;
			_SocketAccessOnly.StartTime = FDateTime::Now().ToUnixTimestamp();
			this->_SocketAccessOnly.Timeout = Timeout >= 0 ? Timeout : 6;
			_SocketAccessOnly.ResolveInfo = _SocketAccessOnly.SocketSubsystem->GetHostByName(TCHAR_TO_ANSI(*Host));
			CheckResolutionAndConnect(_SocketAccessOnly.ResolveInfo);
		}
	};

	_AccessShared.m_Main2SocketTaskQueue.Enqueue(connectTask);
}

void FAsyncSocket::ConnectByIp(const FInternetAddr& Addr, int Timeout)
{
	if (_AccessShared.State >= ESocketState::Connecting)
	{
		EXNETWORKING_LOG(Warning, TEXT("ConnectIp state:%s"), StateToString(_AccessShared.State));
		return;
	}
	_AccessShared.State = ESocketState::Connecting;
	this->_SocketAccessOnly.Timeout = Timeout >= 0 ? Timeout : 6;
	_SocketAccessOnly.StartTime = FDateTime::Now().ToUnixTimestamp();

	_SocketAccessOnly.RecvBuff.clear();
	_SocketAccessOnly.SendBuff.clear();

	EXNETWORKING_LOG(Warning, TEXT("Connect to :%s"), *Addr.ToString(true));
	bool IsValid = _SocketAccessOnly.Socket->Connect(Addr);
	if (!IsValid)
	{
		FString ErrorMsg = "Connect error, socket erro: " + FString(_SocketAccessOnly.SocketSubsystem->GetSocketError(
			_SocketAccessOnly.SocketSubsystem->GetLastErrorCode()));
		HandleConnectError(EConnectCode::CONNECTED_FAILED, ErrorMsg);
	}
}

void FAsyncSocket::Send(const char* Data, size_t Size)
{
	if (_AccessShared.State != ESocketState::Connected)
	{
		EXNETWORKING_LOG(Error, TEXT("Send pkt failed socket state is %s"), StateToString(_AccessShared.State));
		return;
	}

	check(Size > 0);

	//请求包大小超过8K直接丢弃
	if (Size > 8192)
	{
		EXNETWORKING_LOG(Error, TEXT("pkt is more than 8K, drop it. size: %d"), Size);
		return;
	}

	_SocketAccessOnly.SendBuff.append(Data, Size);
	EXNETWORKING_LOG(Log, TEXT("Send pkt, current size: %d, total size:%d"), Size, _SocketAccessOnly.SendBuff.size());

	auto sendTask = [=]() {
		EXNETWORKING_LOG(Log, TEXT("SocketThreadTask: sendTask exec"));

#if PLATFORM_SWITCH
		FRWScopeLock Lock(SocketNifmLock, SLT_ReadOnly);
#endif
		HandleSend();
	};

	_AccessShared.m_Main2SocketTaskQueue.Enqueue(sendTask);
}

void FAsyncSocket::Update()
{
	QUICK_SCOPE_CYCLE_COUNTER(FAsyncSocket_Update);

	// 1. 推送Update任务
	auto updateTask = [this]() {
		// EXNETWORKING_LOG(Log, TEXT("SocketThreadTask: updateTask exec"));

#if PLATFORM_SWITCH
		FRWScopeLock Lock(SocketNifmLock, SLT_ReadOnly);
#endif

		if (_AccessShared.State == ESocketState::Disconnected)
		{
			return;
		}

		if (_AccessShared.State == ESocketState::DNSResolving)
		{
			CheckResolutionAndConnect(_SocketAccessOnly.ResolveInfo);
			return;
		}

		if (_AccessShared.State == ESocketState::Connecting && !CheckIsConnected())
		{
			return;
		}

		if (_AccessShared.State == ESocketState::Connected)
		{
#if PLATFORM_SWITCH
			if (!FPlatformMisc::IsNifmAvailable())
			{
				EXNETWORKING_LOG(Log, TEXT("nifm state change to unavailable"));
				FString ErrorMsg = "nifm state change to unavailable";
				HandleError(ErrorMsg);
				return;
			}
#endif

			if (!HandleRecv())
			{
				return;
			}

			if (_SocketAccessOnly.SendBuff.size() > 0)
			{
				if (!HandleSend())
				{
					return;
				}
			}
			return;
		}

		check(false);
	};

	_AccessShared.m_Main2SocketTaskQueue.Enqueue(updateTask);

	// 2. 执行回调任务
	while (!_AccessShared.m_Socket2MainTaskQueue.IsEmpty())
	{
		TFunction<void()> callBackTask;
		if (_AccessShared.m_Socket2MainTaskQueue.Dequeue(callBackTask))
		{
			callBackTask();
		}
		else
		{
			break;
		}
	}
}

void FAsyncSocket::Close()
{

	auto closeTask = [this]() {
		EXNETWORKING_LOG(Log, TEXT("SocketThreadTask: closeTask exec"));
		_AccessShared.State = ESocketState::Disconnected;
#if PLATFORM_SWITCH
		FRWScopeLock Lock(SocketNifmLock, SLT_ReadOnly);
#endif
		_SocketAccessOnly.Socket->Close();
	};

	_AccessShared.m_Main2SocketTaskQueue.Enqueue(closeTask);
}

bool FAsyncSocket::ProcError()
{
	// 异常只能一个tick处理一个, 因为异常回调可能导致整个对象的析构
	if (!_AccessShared.m_Socket2MainErrorTaskQueue.IsEmpty())
	{
		TFunction<void()> errorCallbackTask;
		if (_AccessShared.m_Socket2MainErrorTaskQueue.Dequeue(errorCallbackTask))
		{
			errorCallbackTask();
			return true;
		}
	}
	return false;
}

const TCHAR* FAsyncSocket::StateToString(ESocketState State)
{
	switch (State)
	{
	case Disconnected:
		return TEXT("Disconnected");
	case DNSResolving:
		return TEXT("DNSResolving");
	case Connecting:
		return TEXT("Connecting");
	case Connected:
		return TEXT("Connected");
	default:
		return TEXT("Unknown State");
	}
}

void FAsyncSocket::HandleConnectError(int RetCode, const FString& ErrorMsg)
{
	_AccessShared.State = ESocketState::Disconnected;
	EXNETWORKING_LOG(Error, TEXT("%s"), *ErrorMsg);

	auto connectCallbackTask = [this, RetCode]() {
		EXNETWORKING_LOG(Log, TEXT("MainThreadTask: connectCallbackTask exec, error"));

		if (_MainAccessOnly.ConnecteCallBack)
		{
			_MainAccessOnly.ConnecteCallBack(false, RetCode);
		}
	};

	_AccessShared.m_Socket2MainErrorTaskQueue.Enqueue(connectCallbackTask);
}

void FAsyncSocket::HandleError(const FString& ErrorMsg)
{
	_AccessShared.State = ESocketState::Disconnected;
	EXNETWORKING_LOG(Error, TEXT("%s"), *ErrorMsg);

	auto errorCallbackTask = [=]() {
		EXNETWORKING_LOG(Log, TEXT("MainThreadTask: errorCallbackTask exec"));

		if (_MainAccessOnly.ErrorCallBack)
		{
			_MainAccessOnly.ErrorCallBack(ErrorMsg.GetCharArray().GetData());
		}
	};

	_AccessShared.m_Socket2MainErrorTaskQueue.Enqueue(errorCallbackTask);
}

void FAsyncSocket::CheckResolutionAndConnect(const FResolveInfo* ResolveInfo)
{
	QUICK_SCOPE_CYCLE_COUNTER(FAsyncSocket_CheckResolutionAndConnect);
	check(ResolveInfo);

	if (ResolveInfo->IsComplete())
	{
		int32 ErrorCode = ResolveInfo->GetErrorCode();
		if (ErrorCode == 0)
		{
			const FInternetAddr* ResolveAddr = &ResolveInfo->GetResolvedAddress();
			TSharedPtr<FInternetAddr> Addr = _SocketAccessOnly.SocketSubsystem->CreateInternetAddr();
			Addr->SetPort(_SocketAccessOnly.AddrPort);
			uint32 IP;
			ResolveAddr->GetIp(IP);
			Addr->SetIp(IP);
			EXNETWORKING_LOG(Warning, TEXT("DNS is complete, peer ip:%s"), *Addr->ToString(true));
			ConnectByIp(*Addr, _SocketAccessOnly.Timeout);
		}
		else
		{
			FString ErrorMsg = "Dns resolution faild, connect error, socket erro: ";
			ErrorMsg += FString(_SocketAccessOnly.SocketSubsystem->GetSocketError((ESocketErrors)ErrorCode));
			HandleConnectError(EConnectCode::DNS_FAILED, ErrorMsg);
		}
	}
	else
	{
		if (FDateTime::Now().ToUnixTimestamp() - _SocketAccessOnly.StartTime > _SocketAccessOnly.Timeout)
		{
			FString ErrorMsg = "DNS resolution timeout";
			HandleConnectError(EConnectCode::DNS_TIMEOUT, ErrorMsg);
		}
	}
}

bool FAsyncSocket::CheckIsConnected()
{
	check(_SocketAccessOnly.Socket);

	if (FDateTime::Now().ToUnixTimestamp() - _SocketAccessOnly.StartTime > _SocketAccessOnly.Timeout)
	{
		FString ErrorMsg = "Connect timeout";
		HandleConnectError(EConnectCode::CONNECTED_TIMEOUT, ErrorMsg);
		return false;
	}

#if PLATFORM_SWITCH
	if (!FPlatformMisc::IsNifmAvailable())
	{
		// EXNETWORKING_LOG(Log, TEXT("nifm is not available"));
		return false;
	}
#endif

	// 收到可写事件说明连接建立成功
	bool bValid = _SocketAccessOnly.Socket->Wait(ESocketWaitConditions::WaitForWrite, 0);
	if (bValid)
	{
		// 同时收到读写事件说明连接建立出错
		bool bRead = _SocketAccessOnly.Socket->Wait(ESocketWaitConditions::WaitForRead, 0);
		if (bRead)
		{
			ESocketErrors ErrorCode = _SocketAccessOnly.SocketSubsystem->GetLastErrorCode();
			FString ErrorMsg =
				"Connect error, socket erro: " + FString(_SocketAccessOnly.SocketSubsystem->GetSocketError(ErrorCode));
			HandleConnectError(EConnectCode::CONNECTED_FAILED, ErrorMsg);
			return false;
		}
		else
		{
			_AccessShared.State = ESocketState::Connected;
			auto connectCallbackTask = [this]() {
				EXNETWORKING_LOG(Log, TEXT("MainThreadTask: connectCallbackTask exec, success"));

				if (_MainAccessOnly.ConnecteCallBack)
				{
					EXNETWORKING_LOG(Log, TEXT("Connect Success"));
					FString Msg = "Connect Success";
					_MainAccessOnly.ConnecteCallBack(true, 0);
				}
			};
			_AccessShared.m_Socket2MainTaskQueue.Enqueue(connectCallbackTask);

			return true;
		}
	}
	return false;
}

bool FAsyncSocket::HandleRecv()
{
	QUICK_SCOPE_CYCLE_COUNTER(FAsyncSocket_HandleRecv);

	check(_SocketAccessOnly.Socket);

	uint8 buffer[1024];
	int ReadSize = 0;
	bool IsValid = false;
	do
	{
		IsValid = _SocketAccessOnly.Socket->Recv(buffer, sizeof(buffer), ReadSize);
		if (ReadSize > 0)
		{
			_SocketAccessOnly.RecvBuff.append((char*)buffer, ReadSize);
			EXNETWORKING_LOG(Log, TEXT("Recv bytes:%d"), ReadSize);
		}
	} while (IsValid && ReadSize > 0);

	if (!IsValid)
	{
		ESocketErrors ErrorCode = _SocketAccessOnly.SocketSubsystem->GetLastErrorCode();
		FString ErrorMsg =
			"Recv error, socket erro: " + FString(_SocketAccessOnly.SocketSubsystem->GetSocketError(ErrorCode));
		HandleError(ErrorMsg);
		return false;
	}

	while (_SocketAccessOnly.RecvBuff.size() > 0)
	{
		std::string RecvPacket = std::move(_SocketAccessOnly.RecvBuff);
		EXNETWORKING_LOG(Log, TEXT("Recv pkt, size:%d"), RecvPacket.size());

		auto recvCallbackTask = [=]() {
			EXNETWORKING_LOG(Log, TEXT("MainThreadTask: recvCallbackTask exec"));

			if (_MainAccessOnly.RecvCallBack)
			{
				_MainAccessOnly.RecvCallBack(RecvPacket.c_str(), RecvPacket.size());
			}
		};
		_AccessShared.m_Socket2MainTaskQueue.Enqueue(recvCallbackTask);
	}

	return true;
}

bool FAsyncSocket::HandleSend()
{
	QUICK_SCOPE_CYCLE_COUNTER(FAsyncSocket_HandleSend);

	check(_SocketAccessOnly.Socket);

	int SendSize = 0;
	bool IsValid = false;

	int iSendLength = _SocketAccessOnly.SendBuff.size();
	if (iSendLength > 16 * 1024)
	{
		EXNETWORKING_LOG(Log, TEXT("Message length is more than 16K %d"), iSendLength);
		iSendLength = 16 * 1024;
	}
	IsValid = _SocketAccessOnly.Socket->Send((const uint8*)_SocketAccessOnly.SendBuff.c_str(), iSendLength, SendSize);
	if (IsValid)
	{
		_SocketAccessOnly.SendBuff.erase(0, SendSize);
		EXNETWORKING_LOG(Log, TEXT("Send bytes:%d"), SendSize);
	}
	else
	{
		ESocketErrors ErrorCode = _SocketAccessOnly.SocketSubsystem->GetLastErrorCode();
		FString ErrorMsg =
			"Send error, socket erro: " + FString(_SocketAccessOnly.SocketSubsystem->GetSocketError(ErrorCode)) +
			FString::FromInt(ErrorCode);
		if (ErrorCode == SE_EWOULDBLOCK)
		{
			EXNETWORKING_LOG(Log, TEXT("SE_EWOULDBLOCK, Send bytes:%d"), SendSize);
		}
		else
		{
			HandleError(ErrorMsg);
		}

		return false;
	}

	return true;
}

void FAsyncSocket::SocketRun()
{
	while (!_AccessShared.m_bShutdown)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 睡10毫秒
		// 不断执行主线程传来的任务
		while (!_AccessShared.m_Main2SocketTaskQueue.IsEmpty())
		{
			TFunction<void()> MainTask;
			if (_AccessShared.m_Main2SocketTaskQueue.Dequeue(MainTask))
			{
				MainTask();
			}
			else
			{
				break;
			}
		}
	}
}
