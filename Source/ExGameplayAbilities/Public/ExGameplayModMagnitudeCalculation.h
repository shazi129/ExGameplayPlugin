#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "ExGameplayModMagnitudeCalculation.generated.h"

UCLASS(BlueprintType, Blueprintable)
class EXGAMEPLAYABILITIES_API UExGameplayModMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

protected:
	//获取运行Effect的AbilitySytemComponent身上的Attribute值
	UFUNCTION(BlueprintCallable)
	float GetOwnerAttributeMagnitude(const FGameplayEffectSpec& EffectSpec, FGameplayAttribute Attribute) const;
};