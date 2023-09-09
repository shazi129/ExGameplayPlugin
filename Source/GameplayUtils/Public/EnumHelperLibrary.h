#pragma once

#include "CoreMinimal.h"
#include "EnumHelperLibrary.generated.h"

UCLASS()
class GAMEPLAYUTILS_API UEnumHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 工具函数， ENetRole转为可识别的字符串
	 */
	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
	static FString NetRoleToString(ENetRole NetRole);

	/**
	 * @brief 工具函数， ENetMode转为可识别的字符串
	*/
	static FString NetModeToString(ENetMode NetMode);
};