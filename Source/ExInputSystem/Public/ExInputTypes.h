#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputActionHandler.h"
#include "ExInputTypes.generated.h"

UENUM(BlueprintType)
enum class EInputBindingType : uint8
{
	//Instanced， 可以直接配置参数， 但可能存在循环引用问题
	E_Instanced		UMETA(DisplayName = "Instanced"),

	//SoftPtr, 延迟加载方式，创建了才加载
	E_SoftReference	UMETA(DisplayName = "SoftReference"),
};

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

	//绑定方式, 建议尽量使用SoftReference的方式，可以有效避免循环引用
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EInputBindingType BindingType = EInputBindingType::E_Instanced;

	//响应逻辑, 不建议直接访问，应使用GetInputHandler函数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, meta = (EditCondition = "BindingType == EInputBindingType::E_Instanced", EditConditionHides))
		UInputActionHandler* InputHandler;

	//响应逻辑, 不建议直接访问，应使用GetInputHandler函数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "BindingType == EInputBindingType::E_SoftReference", EditConditionHides))
		TSoftClassPtr<UInputActionHandler>	InputHandlerClass;

	UInputActionHandler* GetInputHandler(UWorld* ContextWorld);

private:
	UPROPERTY(transient)
		UInputActionHandler* HandlerInstance;
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

UCLASS()
class EXINPUTSYSTEM_API UInputMappingConfigAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/*绑定输入配置*/
	UPROPERTY(EditAnywhere)
		FInputMappingConfig InputMappingConfig;
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



