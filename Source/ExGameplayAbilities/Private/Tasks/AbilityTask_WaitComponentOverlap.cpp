#include "Tasks/AbilityTask_WaitComponentOverlap.h"

UAbilityTask_WaitComponentOverlap* UAbilityTask_WaitComponentOverlap::WaitComponentOverlap(
	UGameplayAbility* OwningAbility,
	UPrimitiveComponent* Component)
{
	UAbilityTask_WaitComponentOverlap* Task = NewAbilityTask<UAbilityTask_WaitComponentOverlap>(OwningAbility);
	Task->WaitComponent = Component;
	return Task;
}

void UAbilityTask_WaitComponentOverlap::OnHitCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		// Construct TargetData
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);

		// Give it a handle and return
		FGameplayAbilityTargetDataHandle	Handle;
		Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnOverlap.Broadcast(Handle);
		}

		// We are done. Kill us so we don't keep getting broadcast messages
		EndTask();
	}
}

void UAbilityTask_WaitComponentOverlap::Activate()
{
	Super::Activate();
	SetWaitingOnAvatar();
	if (WaitComponent)
	{
		WaitComponent->OnComponentHit.AddDynamic(this, &UAbilityTask_WaitComponentOverlap::OnHitCallback);
	}
}

void UAbilityTask_WaitComponentOverlap::OnDestroy(bool AbilityEnded)
{
	if (WaitComponent)
	{
		WaitComponent->OnComponentHit.RemoveDynamic(this, &UAbilityTask_WaitComponentOverlap::OnHitCallback);
	}

	Super::OnDestroy(AbilityEnded);
}
