#include "Tasks/AbilityTask_WaitProjectileStop.h"

UAbilityTask_WaitProjectileStop* UAbilityTask_WaitProjectileStop::WaitProjectileStop(
	UGameplayAbility* OwningAbility,
	UProjectileMovementComponent* ProjectileComponent)
{
	UAbilityTask_WaitProjectileStop* Task = NewAbilityTask<UAbilityTask_WaitProjectileStop>(OwningAbility);
	Task->ProjectileComponent = ProjectileComponent;
	return Task;
}

void UAbilityTask_WaitProjectileStop::OnStopCallback(const FHitResult& ImpactResult)
{

	// Construct TargetData
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(ImpactResult);

	// Give it a handle and return
	FGameplayAbilityTargetDataHandle	Handle;
	Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnStopDelegate.Broadcast(Handle);
	}

	// We are done. Kill us so we don't keep getting broadcast messages
	EndTask();
}

void UAbilityTask_WaitProjectileStop::Activate()
{
	Super::Activate();
	SetWaitingOnAvatar();
	if (ProjectileComponent)
	{
		ProjectileComponent->OnProjectileStop.AddDynamic(this, &UAbilityTask_WaitProjectileStop::OnStopCallback);
	}
}

void UAbilityTask_WaitProjectileStop::OnDestroy(bool AbilityEnded)
{
	if (ProjectileComponent)
	{
		ProjectileComponent->OnProjectileStop.RemoveDynamic(this, &UAbilityTask_WaitProjectileStop::OnStopCallback);
	}

	Super::OnDestroy(AbilityEnded);
}
