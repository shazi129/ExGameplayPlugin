#pragma once 

#include "CoreMinimal.h"
#include "ExAbilitySystemComponent.h"
#include "Tasks/AbilityTask_WaitFrames.h"
#include "AbilityTask_RegisterProvider.generated.h"

UCLASS()
class EXGAMEPLAYABILITIES_API UAbilityTask_RegisterProvider : public UAbilityTask_WaitFramesBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ex Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_RegisterProvider* RegisterProvider(UGameplayAbility* OwningAbility, UExAbilitySystemComponent* ASC,  TScriptInterface<IExAbilityProvider> Provider, int WaitFrames = 1);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void TickTask(float DeltaTime) override;
	virtual bool DoTaskWork() override;

	UPROPERTY(BlueprintAssignable)
	FWaitDelayDelegate	OnFinish;

	UPROPERTY(BlueprintAssignable)
	FWaitDelayDelegate	OnError;

private:
	UExAbilitySystemComponent* ASC;
	TScriptInterface<IExAbilityProvider> Provider;
};