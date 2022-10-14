
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitOverlap.h"
#include "AbilityTask_WaitComponentOverlap.generated.h"

UCLASS()
class UAbilityTask_WaitComponentOverlap : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
		FWaitOverlapDelegate	OnOverlap;

	UFUNCTION()
		void OnHitCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Activate() override;

	/** Wait until an overlap occurs. This will need to be better fleshed out so we can specify game specific collision requirements */
	UFUNCTION(BlueprintCallable, Category = "Ex Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_WaitComponentOverlap* WaitComponentOverlap(UGameplayAbility* OwningAbility, UPrimitiveComponent* Component);

protected:

	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY()
		UPrimitiveComponent* WaitComponent;

};
