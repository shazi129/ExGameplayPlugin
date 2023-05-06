#include "ExInputSubsystem.h"
#include "ExMacros.h"
#include "Kismet/GameplayStatics.h"
#include "ExInputSystemModule.h"

UExInputSubsystem* UExInputSubsystem::GetInputSubsystem(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(ExInputSystemLog, UExInputSubsystem, WorldContextObject);
}

void UExInputSubsystem::TriggerInputTag(const UObject* WorldContextObject, const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("%s error, InputTag is Valid"), *FString(__FUNCTION__));
		return;
	}

	UExInputSubsystem* InputSubsystem = UExInputSubsystem::GetInputSubsystem(WorldContextObject);
	if (!InputSubsystem)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("%s error, cannot find InputSubsystem"), *FString(__FUNCTION__));
		return;
	}
	InputSubsystem->HandleInputEvent(InputTag);
}

FInputHandleEvent* UExInputSubsystem::AddInputEvent(const FGameplayTag& InputTag, int32 Priority)
{
	FInputHandleEvent* NewInputHandleEvent = nullptr;
	for (int EventIndex = 0; EventIndex < InputHandleEvents.Num(); EventIndex++)
	{
		if (Priority == -1 || InputHandleEvents[EventIndex].Priority <= Priority)
		{
			NewInputHandleEvent = &InputHandleEvents.InsertDefaulted_GetRef(EventIndex);
			break;
		}
	}
	if (NewInputHandleEvent == nullptr)
	{
		NewInputHandleEvent = new(InputHandleEvents) FInputHandleEvent();
	}

	NewInputHandleEvent->EventID = EventIDGenerator.fetch_add(1);
	NewInputHandleEvent->InputTag = InputTag;
	return NewInputHandleEvent;
}

int32 UExInputSubsystem::FindInputEventIndex(int32 EventID)
{
	for (int EventIndex = 0; EventIndex < InputHandleEvents.Num(); EventIndex++)
	{
		if (InputHandleEvents[EventIndex].EventID == EventID)
		{
			return EventIndex;
		}
	}
	return -1;
}

bool UExInputSubsystem::RemoveInputEvent(int32 EventID)
{
	int32 EventIndex = FindInputEventIndex(EventID);
	if (EventIndex > 0)
	{
		InputHandleEvents.RemoveAt(EventIndex);
	}
	return true;
}

FInputHandleResult UExInputSubsystem::HandleInputEvent(const FGameplayTag& InputTag)
{
	//通知其他模块处理
	if (OnInputEventReceiveDelegate.IsBound())
	{
		OnInputEventReceiveDelegate.Broadcast(InputTag);
	}

	//依次调用注册的事件
	for (int i = 0; i < InputHandleEvents.Num(); i++)
	{
		if (InputHandleEvents[i].InputTag != InputTag)
		{
			continue;
		}

		FInputHandleDelegate& Delegate = InputHandleEvents[i].InputHandleDelegate;
		if (!Delegate.IsBound())
		{
			continue;
		}

		FInputHandleResult HandleResult = Delegate.Execute(InputTag);
		if (HandleResult.IsHandled)
		{
			if (InputHandleEvents[i].ExecuteOnlyOnce)
			{
				InputHandleEvents.RemoveAt(i);
			}
			return HandleResult;
		}
	}

	return FInputHandleResult();
}
