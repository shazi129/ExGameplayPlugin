#pragma once

#include "ExMathLibrary.generated.h"

UCLASS()
class EXGAMEPLAYLIBRARY_API UExMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ExMathLibrary")
	static bool InBound2D(FVector2D Point, FVector2D Min, FVector2D Max);

	UFUNCTION(BlueprintCallable, Category = "ExMathLibrary")
	static FVector2D CalcRectIntersection(FVector2D StartPoint, FVector2D EndPoint, FVector2D Min, FVector2D Max);

	/* 从FromVector到ToVector的夹角， 逆时针旋转为负 */
	UFUNCTION(BlueprintCallable, Category = "ExMathLibrary")
	static float GetRotationAngle(FVector2D FromVector, FVector2D ToVector);

	/**
	 * @brief 旋转一个向量
	 * @param 要旋转的向量
	 * @param 旋转的角度
	 * @return 旋转后的向量
	*/
	UFUNCTION(BlueprintPure, Category = "ExMathLibrary")
	static FVector RotateVector(const FVector& InVector, const FRotator& InRotator);
};