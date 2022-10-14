#pragma once

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilityTask_ServerOnlyPlayMontageAndWait.generated.h"

UCLASS()
class UAbilityTask_ServerOnlyPlayMontageAndWait : public UAbilityTask_PlayMontageAndWait
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "ServerOnly_PlayMontageAndWait",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ServerOnlyPlayMontageAndWait* CreateServerOnlyPlayMontageAndWaitProxy(
				UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* Montage,
				float PlayRate = 1.f, FName PlayStartSection = NAME_None, bool StopWhenAbilityEnds = true,
				float AnimRootMotionScale = 1.f, float StartTime = 0.f);

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityEnded) override;
};