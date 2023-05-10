#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "ExGameplayAbilityTargetTypes.h"
#include "ExGameplayAbilityLibrary.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FTestStruct
{
	GENERATED_BODY()
};

UCLASS()
class EXGAMEPLAYABILITIES_API UExGameplayAbilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static TArray<int64> GetInt64ArrayFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index=0);

	UFUNCTION(BlueprintPure)
	static FInstancedStruct GetInstancedStructFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index = 0);

};
