#pragma once
#include "CoreMinimal.h"
#include "Networking.h"
#include <string>
#include <string.h>
#include <thread>
#include "Containers/Queue.h"
#include "Templates/Atomic.h"
#include "Templates/SharedPointer.h"

enum ESocketState
{
	Disconnected,
	DNSResolving,
	Connecting,
	Connected,
};

enum EConnectCode
{
	SUCCESS,
	DNS_TIMEOUT,
	DNS_FAILED,
	CONNECTED_TIMEOUT,
	CONNECTED_FAILED,
};

class EXNETWORKING_API FAsyncSocket : public TSharedFromThis<FAsyncSocket>
{
public:
	FAsyncSocket();
	~FAsyncSocket();

	// 每次重连时需要调用
	void InitAsyncSocket();
	void Connect(const FString& Host, uint16 Port, int Timeout /*second*/);
	void Send(const char* Data, size_t Size);
	void Update();
	void Close();
	void SetRecvCallBack(TFunction<void(const char*, size_t)>& CallBack) { _MainAccessOnly.RecvCallBack = CallBack; }
	void SetErrorCallBack(TFunction<void(const TCHAR*)>& CallBack) { _MainAccessOnly.ErrorCallBack = CallBack; }
	void SetConnecteCallBack(TFunction<void(bool, int)>& CallBack) { _MainAccessOnly.ConnecteCallBack = CallBack; }
	ESocketState GetState() { return _AccessShared.State; }
	bool ProcError();

private:
	void ConnectByIp(const FInternetAddr& Addr, int Timeout /*second*/);

	const TCHAR* StateToString(ESocketState State);
	void HandleConnectError(int RetCode, const FString& ErrorMsg);
	void HandleError(const FString& ErrorMsg);
	void CheckResolutionAndConnect(const FResolveInfo* ResolveInfo);
	bool CheckIsConnected();
	bool HandleRecv();
	bool HandleSend();
	void SocketRun();  // Socket线程循环

private:
	// 只有Socket线程读写的成员变量
	struct
	{
		ISocketSubsystem* SocketSubsystem = nullptr;             // UE4提供的抽象Socket系统
		FSocket* Socket = nullptr;                               // 实际通信的Socket
		const FResolveInfo* ResolveInfo = nullptr;               // 域名解析信息
		FString AddrHost;                                        // 后台IP
		uint16 AddrPort;                                         // 后台Port
		std::string RecvBuff;                                    // 接收Buff
		std::string SendBuff;                                    // 发送Buff
		int Timeout = 0;                                         // DNS解析超时时间
		int StartTime = 0;                                       // DNS解析开始时间

	} _SocketAccessOnly;

	// 只有Main线程读写的成员变量
	struct
	{
		std::thread m_thread;                               // 执行线程
		TFunction<void(const char*, size_t)> RecvCallBack;  // 收到回包时的回调
		TFunction<void(const TCHAR*)> ErrorCallBack;        // 发生错误时的回调
		TFunction<void(bool, int)> ConnecteCallBack;        // 连接出结果的回调
		bool bFirstReset;                                   // 是否第一次初始化
	} _MainAccessOnly;

	// 共享的成员变量
	struct
	{
		// 主线程&&Socket线程同时读写的成员变量
		TAtomic<bool> m_bShutdown;                              // 是否停止线程
		TQueue<TFunction<void()>> m_Main2SocketTaskQueue;       // 主线程->Socket线程的任务队列
		TQueue<TFunction<void()>> m_Socket2MainTaskQueue;       // Socket线程->主线程的任务队列
		TQueue<TFunction<void()>> m_Socket2MainErrorTaskQueue;  // Socket线程->主线程的异常任务队列
		TAtomic<ESocketState> State;                            // Socket状态
	} _AccessShared;
};
