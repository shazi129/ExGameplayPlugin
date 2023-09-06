#include "Tasks/AbilityTask_WaitFrames.h"
#include "ExGameplayAbilitiesModule.h"

void UAbilityTask_WaitFramesBase::Activate()
{
	CurrentFrames = -1;
	if (WaitFrames < 0)
	{
		WaitFrames = 0;
	}
	bTickingTask = true;
}

void UAbilityTask_WaitFramesBase::TickTask(float DeltaTime)
{
	CurrentFrames++;
	if (CurrentFrames != WaitFrames)
	{
		return;
	}
	DoTaskWork();
}

bool UAbilityTask_WaitFramesBase::DoTaskWork()
{
	EndTask();
	return true;
}

UAbilityTask_WaitForFrames* UAbilityTask_WaitForFrames::WaitForFrames(UGameplayAbility* OwningAbility, int WaitFrames)
{
	UAbilityTask_WaitForFrames* Task = NewAbilityTask<UAbilityTask_WaitForFrames>(OwningAbility);
	Task->WaitFrames = WaitFrames;
	return Task;
}

bool UAbilityTask_WaitForFrames::DoTaskWork()
{
	OnFinish.Broadcast();
	EndTask();
	return true;
}
