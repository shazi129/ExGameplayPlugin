#pragma once

#include "InputActionHandler.h"
#include "AbilitySystemComponent.h"
#include "InputHandler_ActivateAbility.generated.h"

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_ActivateAbility : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> AbilityClass;

protected:
	UAbilitySystemComponent* GetAbilitySystemComponent();
};