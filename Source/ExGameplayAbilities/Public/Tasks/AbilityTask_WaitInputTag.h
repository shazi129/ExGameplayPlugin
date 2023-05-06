#pragma once

#include "CoreMinimal.h"
#include "ExInputSubsystem.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitInputTag.generated.h"

UCLASS()
class UAbilityTask_WaitInputTag : public UAbilityTask
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Ex Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_WaitInputTag* WaitInputTag(UGameplayAbility* OwningAbility, FGameplayTag InputTag, int32 InputPriority);

	UPROPERTY(BlueprintAssignable)
		FInputHandleMulticastDelegate	 OnInputTagReceive;

protected:

	virtual void OnDestroy(bool AbilityEnded) override;

	UFUNCTION()
	virtual FInputHandleResult HandleInputTag(const FGameplayTag& InInputTag);

	FGameplayTag InputTag;
	int32 InputPriority;

	int32 BindEventID;
};