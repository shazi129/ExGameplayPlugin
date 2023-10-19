#include "MessageCenter/MessageCenterSubsystem.h"
#include "Macros/SubsystemMacros.h"
#include "GameplayUtilsModule.h"

UMessageCenterSubsystem* UMessageCenterSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogGameplayUtils, UMessageCenterSubsystem, WorldContextObject);
}

void UMessageCenterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMessageCenterSubsystem::Deinitialize()
{
	Super::Deinitialize();
}


void UMessageCenterSubsystem::SendMsgToServer(const FGameplayMessage& Message)
{
	if (RPCFunctionProvider != nullptr)
	{
		RPCFunctionProvider->SendMsgToServer(Message);
	}
}

void UMessageCenterSubsystem::SendMsgToClient(const FGameplayMessage& Message)
{
	if (RPCFunctionProvider != nullptr)
	{
		RPCFunctionProvider->SendMsgToClient(Message);
	}
}

void UMessageCenterSubsystem::RegisterRPCFunctionProvider(TScriptInterface<IRPCFunctionProvider> Provider)
{
	RPCFunctionProvider = Provider.GetInterface();
}

void UMessageCenterSubsystem::UnregisterRPCFunctionProvider(TScriptInterface<IRPCFunctionProvider> Provider)
{
	if (RPCFunctionProvider != Provider.GetInterface())
	{
		RPCFunctionProvider = nullptr;
	}
}

FMessageReceivedDelegateInfo& UMessageCenterSubsystem::GetMessageReceivedDelegateInfo(FGameplayTag MsgTag)
{
	return OnMessageReceivedDelegateMap.FindOrAdd(MsgTag);
}

void UMessageCenterSubsystem::OnMessageReceived(const FGameplayMessage& Message)
{
	if (!Message.MsgTag.IsValid())
	{
		return;
	}

	auto DelegateInfoPtr = OnMessageReceivedDelegateMap.Find(Message.MsgTag);
	if (DelegateInfoPtr)
	{
		DelegateInfoPtr->Delegate.Broadcast(Message);
	}
}


