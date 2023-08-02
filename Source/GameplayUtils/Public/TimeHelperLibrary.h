#pragma once

/**
 * 时间相关工具函数
 */
#include "CoreMinimal.h"
#include "TimeHelperLibrary.generated.h"

UCLASS()
class GAMEPLAYUTILS_API UTimeHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 获取UTC时间戳
	 * @return 时间戳，秒级
	*/
	UFUNCTION(BlueprintPure, Category = "Time Helper")
	static int64 GetTimestamp(); //统一用的UTC时间

	/**
	 * @brief 获取UTC时间戳
	 * @return 时间戳，毫秒级
	*/
	UFUNCTION(BlueprintPure, Category = "Time Helper")
	static int64 GetTimestampMs();

	/**
	 * @brief 获取当前时区
	 * @return 时区
	*/
	UFUNCTION(BlueprintPure, Category = "Time Helper")
	static int GetTimeZone(); //当地时区

	/**
	 * @brief 获取当前帧开始的时间
	 * @return 时间，秒级
	*/
	UFUNCTION(BlueprintPure, Category = "Time Helper")
	static double GetCurrentFrameTime();
};