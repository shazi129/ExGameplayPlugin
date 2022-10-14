#pragma once

#include "CoreMinimal.h"
#include "InputActionHandler.h"
#include "GameplayTags.h"
#include "Abilities/GameplayAbility.h"
#include "InputHandler_TriggerGameAbility.generated.h"

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_TriggerGameAbility : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UGameplayAbility> AbilityClass;
};