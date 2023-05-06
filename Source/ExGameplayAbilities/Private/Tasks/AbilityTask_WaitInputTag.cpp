#include "Tasks/AbilityTask_WaitInputTag.h"

UAbilityTask_WaitInputTag* UAbilityTask_WaitInputTag::WaitInputTag(UGameplayAbility* OwningAbility, FGameplayTag InputTag, int32 InputPriority)
{
	UAbilityTask_WaitInputTag* Task = NewAbilityTask<UAbilityTask_WaitInputTag>(OwningAbility);
	Task->InputTag = InputTag;
	Task->InputPriority = InputPriority;
	Task->BindEventID = 0;
	return Task;
}

void UAbilityTask_WaitInputTag::Activate()
{
	Super::Activate();

	UExInputSubsystem* InputSubsystem = UExInputSubsystem::GetInputSubsystem(this);
	if (InputSubsystem)
	{
		FInputHandleEvent* InputEvent = InputSubsystem->AddInputEvent(InputTag, InputPriority);
		InputEvent->ExecuteOnlyOnce = true;
		InputEvent->InputHandleDelegate.BindDynamic(this, &UAbilityTask_WaitInputTag::HandleInputTag);
		BindEventID = InputEvent->EventID;
	}
}

void UAbilityTask_WaitInputTag::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	UExInputSubsystem* InputSubsystem = UExInputSubsystem::GetInputSubsystem(this);
	if (InputSubsystem)
	{
		InputSubsystem->RemoveInputEvent(BindEventID);
	}
}

FInputHandleResult UAbilityTask_WaitInputTag::HandleInputTag(const FGameplayTag& InInputTag)
{
	FInputHandleResult Result;
	Result.IsHandled = true;

	if (OnInputTagReceive.IsBound())
	{
		OnInputTagReceive.Broadcast(InInputTag);
	}
	return Result;
}

