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

	// ÿ������ʱ��Ҫ����
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
	void SocketRun();  // Socket�߳�ѭ��

private:
	// ֻ��Socket�̶߳�д�ĳ�Ա����
	struct
	{
		ISocketSubsystem* SocketSubsystem = nullptr;             // UE4�ṩ�ĳ���Socketϵͳ
		FSocket* Socket = nullptr;                               // ʵ��ͨ�ŵ�Socket
		const FResolveInfo* ResolveInfo = nullptr;               // ����������Ϣ
		FString AddrHost;                                        // ��̨IP
		uint16 AddrPort;                                         // ��̨Port
		std::string RecvBuff;                                    // ����Buff
		std::string SendBuff;                                    // ����Buff
		int Timeout = 0;                                         // DNS������ʱʱ��
		int StartTime = 0;                                       // DNS������ʼʱ��

	} _SocketAccessOnly;

	// ֻ��Main�̶߳�д�ĳ�Ա����
	struct
	{
		std::thread m_thread;                               // ִ���߳�
		TFunction<void(const char*, size_t)> RecvCallBack;  // �յ��ذ�ʱ�Ļص�
		TFunction<void(const TCHAR*)> ErrorCallBack;        // ��������ʱ�Ļص�
		TFunction<void(bool, int)> ConnecteCallBack;        // ���ӳ�����Ļص�
		bool bFirstReset;                                   // �Ƿ��һ�γ�ʼ��
	} _MainAccessOnly;

	// ����ĳ�Ա����
	struct
	{
		// ���߳�&&Socket�߳�ͬʱ��д�ĳ�Ա����
		TAtomic<bool> m_bShutdown;                              // �Ƿ�ֹͣ�߳�
		TQueue<TFunction<void()>> m_Main2SocketTaskQueue;       // ���߳�->Socket�̵߳��������
		TQueue<TFunction<void()>> m_Socket2MainTaskQueue;       // Socket�߳�->���̵߳��������
		TQueue<TFunction<void()>> m_Socket2MainErrorTaskQueue;  // Socket�߳�->���̵߳��쳣�������
		TAtomic<ESocketState> State;                            // Socket״̬
	} _AccessShared;
};
