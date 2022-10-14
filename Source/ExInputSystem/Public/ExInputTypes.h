#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputActionHandler.h"
#include "GameplayTagContainer.h"
#include "ExInputTypes.generated.h"


USTRUCT(BlueprintType)
struct EXINPUTSYSTEM_API FInputBindingConfig
{
	GENERATED_BODY()

public:
	//需要绑定的Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UInputAction> InputAction;

	//如何触发
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ETriggerEvent TriggerEvent;

	//响应逻辑
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
		UInputActionHandler* InputHandler;
};

USTRUCT(BlueprintType)
struct EXINPUTSYSTEM_API FInputMappingConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int InputPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FInputBindingConfig> InputBindings;
};

//一个输入绑定后的结果
USTRUCT(BlueprintType)
struct EXINPUTSYSTEM_API FInputMappingResult
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		UInputMappingContext* InputMappingContext;

	UPROPERTY(BlueprintReadWrite)
		TArray<int> InputBindHandlers;
};


USTRUCT(BlueprintType)
struct EXINPUTSYSTEM_API FInputHandleResult
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		bool IsHandled = false;
};

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FInputHandleResult, FInputHandleDelegate, const FGameplayTag&, InputTag);

USTRUCT(BlueprintType)
struct EXINPUTSYSTEM_API FInputHandleEvent
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag InputTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FInputHandleDelegate InputHandleDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Priority = 0;
};