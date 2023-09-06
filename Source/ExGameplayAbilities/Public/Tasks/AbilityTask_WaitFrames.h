#pragma once 

#include "CoreMinimal.h"
#include "ExAbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilityTask_WaitFrames.generated.h"

UCLASS()
class EXGAMEPLAYABILITIES_API UAbilityTask_WaitFramesBase : public UAbilityTask
{
	GENERATED_BODY()

public:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	virtual bool DoTaskWork();

protected:
	int WaitFrames;
	int CurrentFrames;
};

UCLASS()
class EXGAMEPLAYABILITIES_API UAbilityTask_WaitForFrames : public UAbilityTask_WaitFramesBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ex Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitForFrames* WaitForFrames(UGameplayAbility* OwningAbility, int WaitFrames);

	UPROPERTY(BlueprintAssignable)
	FWaitDelayDelegate	OnFinish;

	virtual bool DoTaskWork() override;
};
