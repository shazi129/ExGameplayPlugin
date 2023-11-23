#pragma once

#include "CoreMinimal.h"
#include "InputActionHandler.h"
#include "GameplayTags.h"
#include "InstancedStruct.h"
#include "Abilities/GameplayAbility.h"
#include "ExAbilitySystemComponent.h"
#include "InputHandler_ActivateAbilityByTag.generated.h"

namespace AbilityInpuHandlerHelper
{
	UExAbilitySystemComponent* GetAbilitySystem(UObject* SourceObject);
	void ActivateAbilityByCategory(UExAbilitySystemComponent* ASC, const FGameplayTag& CategoryTag, bool ToServer);
}

UCLASS()
class EXGAMEPLAYABILITIES_API UInputHandler_ActivateAbilityByTag : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInstancedStruct ActivateData;

	//是否需要将这个行为发送到server端， 默认在主控端执行
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ToServer = false;
};