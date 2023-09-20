#pragma once

#include "CoreMinimal.h"
#include "InputActionHandler.h"
#include "GameplayTags.h"
#include "Abilities/GameplayAbility.h"
#include "InputHandler_ActivateAbilityByCategory.generated.h"

UCLASS()
class EXGAMEPLAYABILITIES_API UInputHandler_ActivateAbilityByCategory : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
	virtual void CopyFrom(const UInputActionHandler* Other);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag CategoryTag;
};