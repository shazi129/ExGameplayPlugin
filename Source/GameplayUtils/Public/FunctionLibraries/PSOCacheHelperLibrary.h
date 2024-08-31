#pragma once

#include "PSOCacheHelperLibrary.generated.h"


/**
 * 用于处理PSO缓存处理相关
 */
UCLASS()
class GAMEPLAYUTILS_API UPSOCacheHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PSOCacheHelperLibrary")
	static bool EnableShaderPipelineCache(bool bEnable);
	
	UFUNCTION(BlueprintCallable, Category = "PSOCacheHelperLibrary")
	static bool EnableLogPSO(bool bEnable);

	// UFUNCTION(BlueprintCallable, Category = "PSOCacheHelperLibrary")
	// static bool LoadShaderPipelineCache();

	UFUNCTION(BlueprintCallable, Category = "PSOCacheHelperLibrary")
	static bool SavePipelineFileCache();
};
