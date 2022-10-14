
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitOverlap.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilityTask_WaitProjectileStop.generated.h"

UCLASS()
class UAbilityTask_WaitProjectileStop : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
		FWaitOverlapDelegate	OnStopDelegate;

	UFUNCTION()
		void OnStopCallback(const FHitResult& ImpactResult);

	virtual void Activate() override;

	/** Wait until an overlap occurs. This will need to be better fleshed out so we can specify game specific collision requirements */
	UFUNCTION(BlueprintCallable, Category = "Ex Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_WaitProjectileStop* WaitProjectileStop(
			UGameplayAbility* OwningAbility,
			UProjectileMovementComponent* ProjectileComponent);

protected:

	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY()
		UProjectileMovementComponent* ProjectileComponent;

};
