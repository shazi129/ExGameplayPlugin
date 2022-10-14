
#pragma once

#include "CoreMinimal.h"
#include "ExGameplayAbility.h"
#include "ExAbilityProvider.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class EXGAMEPLAYABILITIES_API UExAbilityProvider : public UInterface
{
	GENERATED_BODY()
};

class IExAbilityProvider
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "ExAbility")
		virtual void CollectAbilitCases(TArray<FExAbilityCase>& AbilityConfsToAppend) const = 0;
};
