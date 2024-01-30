#pragma once

#pragma once

/**
 * 运动相关工具函数
 */
#include "CoreMinimal.h"
#include "MovementHelperLibrary.generated.h"

 //相对运动信息
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FMovementBasedData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bHasBasedTransform = false;

	UPROPERTY(BlueprintReadWrite)
	FTransform BasedTransform;

	UPROPERTY(BlueprintReadWrite)
	FTransform RelativeTransform;
};

//物体运动信息
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FMovementData
{
	GENERATED_BODY()

	//本次移动数据的时间
	UPROPERTY(BlueprintReadWrite)
	int64 Timestamp;

	//速度
	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;

	//加速度
	UPROPERTY(BlueprintReadWrite)
	FVector Acceleration;

	//世界坐标下的Transform
	UPROPERTY(BlueprintReadWrite)
	FTransform Transform;

	//工具函数
	FMovementData() { Reset(); }
	void Reset();
	FString ToString() const;
};

//运动预测参数
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FPredictMovementParmeter
{
	GENERATED_BODY();

	//最大速度, 0表示不做限制
	UPROPERTY(BlueprintReadWrite)
	float MaxSpeed = 0.0f;

	//相对运动信息
	UPROPERTY(BlueprintReadWrite)
	FMovementBasedData BasedData;
};

UCLASS()
class GAMEPLAYUTILS_API UMovementHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static FMovementData PredictMovement(const FMovementData& MovementData, const FPredictMovementParmeter& PredictParamter, float DeltaSeconds);
};