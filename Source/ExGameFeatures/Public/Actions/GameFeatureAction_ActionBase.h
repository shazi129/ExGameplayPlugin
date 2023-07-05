#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_ActionBase.generated.h"

UCLASS(Abstract)
class UGameFeatureAction_ActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	static FString ActionToString(UGameFeatureAction* Action);

public:
	virtual FString ToString() const;
};