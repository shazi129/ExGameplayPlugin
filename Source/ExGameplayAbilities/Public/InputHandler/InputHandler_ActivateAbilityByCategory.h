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

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag CategoryTag;

	//是否需要将这个行为发送到server端， 默认在主控端执行
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ToServer = false;
};