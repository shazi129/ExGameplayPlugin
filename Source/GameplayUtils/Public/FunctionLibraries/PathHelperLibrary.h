#pragma once

/**
 * 包含文件，包路径，包名相关工具函数
 */
#include "CoreMinimal.h"
#include "PathHelperLibrary.generated.h"

UCLASS()
class GAMEPLAYUTILS_API UPathHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
		* @brief 获取Object的完整包名，例如：/Game/FPSGame/GameBase/BP_Character,等同于Object->GetPackage()->GetName()
		* 但会过滤Editor环境下的PIE前缀，例如在PIE模式下，一个Level的包名为：/Game/FPSGame/Maps/UEDPIE_0_Login
		* 通过GetPackageFullName获取的是：/Game/FPSGame/Maps/Login
		*
		* @param Object 目标Object
		* @return 完整包名
	*/
	UFUNCTION(BlueprintPure, Category = "Path Helper")
	static FString GetPackageFullName(const UObject* Object);

	/**
	 * @brief 获取系统绝对路径，例如：../../Saved/Log -> C:/ProjectPath/Saved/Log
	 * @param RelativePath 
	 * @return 
	*/
	UFUNCTION(BlueprintPure, Category = "Path Helper")
	static FString GetExternalAbsolutePath(const FString& RelativePath);
};