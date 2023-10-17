#include "MessageCenter/MessageCenterSubsystem.h"

UMessageCenterSubsystem* UMessageCenterSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	return nullptr;
}

void UMessageCenterSubsystem::SendMessage(const FGameplayTag& MessageTag, const FInstancedStruct& MessageBody, EMessageDirection Direction)
{
	
}

int32 UMessageCenterSubsystem::RegistListener(const FGameplayTag& MessageTag, UObject* Object, FName FunctionName, bool MatchExactly)
{
	FScriptDelegate ListenDelegate;
	ListenDelegate.BindUFunction(Object, FunctionName);
	ListenDelegates.FindOrAdd(MessageTag).Add(ListenDelegate);
	return 0;
}

void UMessageCenterSubsystem::UnregistListener()
{
}

void UMessageCenterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMessageCenterSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

