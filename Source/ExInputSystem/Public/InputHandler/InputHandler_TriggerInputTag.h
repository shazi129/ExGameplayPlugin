#pragma once

#include "CoreMinimal.h"
#include "InputBindingAction.h"
#include "GameplayTags.h"
#include "InputHandler_TriggerInputTag.generated.h"

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_TriggerInptutTag : public UInputBindingActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag;
};