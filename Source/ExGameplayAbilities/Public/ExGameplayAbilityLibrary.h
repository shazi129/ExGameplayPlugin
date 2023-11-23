#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "ExGameplayAbilityTargetTypes.h"
#include "ExGameplayAbility.h"
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
	static TArray<int64> GetInt64ArrayFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index = 0);

	UFUNCTION(BlueprintPure)
	static FInstancedStruct GetInstancedStructFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index = 0);

	UFUNCTION(BlueprintPure)
	static bool IsAbilityCaseValid(const FExAbilityCase& AbilityCase);

	//通过Gameplay Effect Spec获取ASC
	UFUNCTION(BlueprintCallable)
	static UAbilitySystemComponent* GetInstigatorASCWithGESpec(const FGameplayEffectSpec& Spec);

	UFUNCTION(BlueprintCallable)
	static FActiveGameplayEffectHandle ApplyGameplayEffectToSelfAndSetSourceObject(UAbilitySystemComponent* Target, TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle EffectContext,
	                                                                               AActor* Attacker, float InAttackCoefficient, float InHitCoefficient, float InRelativeVelocity, float InWeight);

	UFUNCTION(BlueprintCallable)
	static FActiveGameplayEffectHandle ApplyGameplayEffectClass(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> EffectClass);
};
