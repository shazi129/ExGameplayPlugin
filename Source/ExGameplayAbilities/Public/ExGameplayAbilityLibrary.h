#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "ExGameplayAbilityLibrary.generated.h"

UCLASS()
class EXGAMEPLAYABILITIES_API UExGameplayAbilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static TArray<int64> GetInt64ArrayFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index=0);

};
