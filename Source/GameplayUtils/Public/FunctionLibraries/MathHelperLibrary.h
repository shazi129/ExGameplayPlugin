#pragma once

#include "MathHelperLibrary.generated.h"

UCLASS()
class GAMEPLAYUTILS_API UMathHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 随机2D圆形区域中一个点，对FMath::RandPointInCircle的封装
	 * @param CircleRadius 半径
	 * @param CircleCenter  圆心
	 * @return 随机的点
	*/
	UFUNCTION(BlueprintPure)
	static FVector2D RandPointInCircle(FVector2D CircleCenter, float CircleRadius);

	/**
	 * @brief 将一个线段分成N份
	 * @param StartPoint 线段起始点
	 * @param EndPoint  线段结束点
	 * @param Num 
	 * @return 
	*/
	UFUNCTION(BlueprintPure)
	static TArray<FVector> SplitLineByLength(const FVector& StartPoint, const FVector& EndPoint, float UnitLength);

	/**
	 * @brief 将一个Rotator再旋转从StartVector到EdnVector的角度
	 * @param Rotator 
	 * @param StartVector 
	 * @param EndVector 
	 * @return 
	*/
	UFUNCTION(BlueprintPure)
	static FRotator MakeRotBetweenVectors(const FRotator& Rotator, const FVector& StartVector, const FVector& EndVector);
};