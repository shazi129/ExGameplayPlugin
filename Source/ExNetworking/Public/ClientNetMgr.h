#pragma once

#include "CoreMinimal.h"
#include "AsyncSocket.h"
#include "ClientNetMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConnectedNotifyDelegate, bool, bSuccess , int32,  Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReceivedDataNotifyDelegate, const TArray<uint8>&, data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FErrorNotifyDelegate, FString, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDisconnectedNotifyDelegate, int32, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNetworkEventNotifyDelegate, int32, EventID, int32, EventParam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FStateChangeNotifyDelegate, int32, State, int32, EventParam0, int32,EventParam1, int32,EventParam2);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHttpDNSNotifyDelegate, bool,iSuccess, FString,hostURL, FString, ipURL);

UCLASS(BlueprintType)
class EXNETWORKING_API UClientNetMgr : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

public:
	~UClientNetMgr();

	virtual	void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UClientNetMgr, STATGROUP_Tickables); }

	UFUNCTION(BlueprintCallable)
	virtual void Shutdown();

	UFUNCTION(BlueprintCallable)
	virtual void Connect(int32 Timeout);

	UFUNCTION(BlueprintCallable)
	virtual void Disconnect();

	virtual void Send(const uint8* Data, uint32 Len);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BP_Send(const FString& msg);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BP_SendArray(const TArray<uint8>& msg);

	UFUNCTION(BlueprintCallable)
	virtual void InitConnection(FString URL);

	//-------------一些回调----------------------------
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BP_OnConnected(bool bSuccessful, int ErrorCode);
	virtual void OnConnected(bool bSuccessful, int ErrorCode);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BP_OnReceiveData(UPARAM(ref) TArray<uint8>& data);
	virtual void OnReceiveData(const char* Data, size_t Size);

public:
	UPROPERTY(BlueprintAssignable)
	FConnectedNotifyDelegate ConnectedNotifyDelegate;

	UPROPERTY(BlueprintAssignable)
	FStateChangeNotifyDelegate StateChangeNotifyDelegate;

	UPROPERTY(BlueprintAssignable)
	FReceivedDataNotifyDelegate ReceivedDataNotifyDelegate;

	UPROPERTY(BlueprintAssignable)
	FErrorNotifyDelegate ErrorNotifyDelegate;

	UPROPERTY(BlueprintAssignable)
	FDisconnectedNotifyDelegate DisconnectedNotifyDelegate;

	UPROPERTY(BlueprintAssignable)
	FNetworkEventNotifyDelegate NetworkEventNotifyDelegate;

	UPROPERTY(BlueprintAssignable)
	FHttpDNSNotifyDelegate HttpDNSNotifyDelegate;

	TFunction<void()> SendHeartBeatCallBack;

protected:
	virtual bool CheckIfHeartTimeout();
	virtual void HandleAppResumed();

private:
	FString _connectioURL;
	FString _host;
	int32 _port;

	TUniquePtr<FAsyncSocket> m_pSocket = nullptr;
	

	//----------收协议超时异常---------
	int32 WaitingMsgID;
	int32 SendingMsgID;
	int32 RecivedMsgID;
	float WaitingTime;

	//切到后台的处理
	FDelegateHandle OnAppResumedDelegate;
	bool bAppResumedReconnect = false;

	//---------时间相关-----------------
public:
	/** 等待协议回包时间，超出时间会抛异常*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitRecvTime;

private:
	int LastActiveTime = 0;
	int HeartBeatTimeout = 60;
	int LastHeartBeatTime = 0;
	int HeartBeatInterval = 8;

private:
	std::string _currentRecvBuff;
};