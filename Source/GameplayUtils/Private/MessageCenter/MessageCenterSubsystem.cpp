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

void UMessageCenterSubsystem::SendMessage(const FGameplayMessage& Message, EMessageDirection Direction)
{
	if (!Message.MsgTag.IsValid())
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, Invalid Message Tag"), *FString(__FUNCTION__));
		return;
	}

	if (Direction == EMessageDirection::E_None)
	{
		auto DelegateInfoPtr = OnMessageReceivedDelegateMap.Find(Message.MsgTag);
		if (DelegateInfoPtr)
		{
			DelegateInfoPtr->Delegate.Broadcast(Message);
		}
	}
	else if (RPCFunctionProvider != nullptr)
	{
		if (Direction == EMessageDirection::E_ToClient)
		{
			RPCFunctionProvider->SendMsgToClient(Message);
		}
		else if (Direction == EMessageDirection::E_ToServer)
		{
			RPCFunctionProvider->SendMsgToServer(Message);
		}
	}
}

void UMessageCenterSubsystem::SendBooleanMessage(const FGameplayTag& MsgTag, bool Value, UObject* SourceObject, EMessageDirection Direction)
{
	FGameplayMessage Message(MsgTag, SourceObject);
	FBooleanMessagBody BooleanMessageBody(Value);
	SendMessage(Message, Direction);
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

