#include "ExInputSubsystem.h"
#include "Kismet/GameplayStatics.h"

UExInputSubsystem* UExInputSubsystem::GetInputSubsystem(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UExInputSubsystem>();
}


FInputHandleEvent UExInputSubsystem::CreateInputEvent(const FGameplayTag& GameplayTag, const FInputHandleDelegate& Delegate)
{
	FInputHandleEvent NewInputHandleEvent;
	NewInputHandleEvent.InputTag = GameplayTag;
	NewInputHandleEvent.InputHandleDelegate = Delegate;
	return NewInputHandleEvent;
}


void UExInputSubsystem::AddInputEvent(const FInputHandleEvent& InputHandleEvent)
{
	FInputHandleEvent* NewInputHandleEvent = new(InputHandleEvents) FInputHandleEvent();
	NewInputHandleEvent->InputTag = InputHandleEvent.InputTag;
	NewInputHandleEvent->InputHandleDelegate = InputHandleEvent.InputHandleDelegate;
}

void UExInputSubsystem::RemoveInputEvent(const FInputHandleEvent& InputHandleEvent)
{
	for (int i = InputHandleEvents.Num() - 1; i >= 0; i--)
	{
		if (InputHandleEvents[i].InputHandleDelegate == InputHandleEvent.InputHandleDelegate
			&& InputHandleEvents[i].InputTag == InputHandleEvent.InputTag)
		{
			InputHandleEvents.RemoveAt(i);
		}
	}
}

FInputHandleResult UExInputSubsystem::HandleInputEvent(const FGameplayTag& InputTag)
{
	for (int i = 0; i < InputHandleEvents.Num(); i++)
	{
		if (InputHandleEvents[i].InputTag == InputTag)
		{
			FInputHandleDelegate& Delegate = InputHandleEvents[i].InputHandleDelegate;
			if (Delegate.IsBound())
			{
				FInputHandleResult HandleResult = Delegate.Execute(InputTag);
				if (HandleResult.IsHandled)
				{
					return HandleResult;
				}
			}
		}
	}
	return FInputHandleResult();
}
