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
};