
#pragma once

#include "CoreMinimal.h"
#include "ExGameplayAbility.h"
#include "ExAbilityProvider.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAbilityProviderInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int ProviderType;

	UPROPERTY()
	int ProviderID;

	UPROPERTY()
	UObject* ProviderObject;

	void Reset();
	bool IsValid() const;
	bool operator==(const FAbilityProviderInfo& Other) const;

	FString ToString() const;

	FAbilityProviderInfo();
	FAbilityProviderInfo(UObject* InProviderObject);
	FAbilityProviderInfo(int InProviderType, int InProviderID ,UObject* InProviderObject);
};

USTRUCT()
struct EXGAMEPLAYABILITIES_API FCollectedAbilityInfo
{
	GENERATED_BODY()

	//Provider Info
	FAbilityProviderInfo ProviderInfo;

	//该Provider提供的Ability
	TArray<FExAbilityCase> AbilityCases;

	void Reset();
	bool IsVaild() const;
};

UINTERFACE(Blueprintable)
class EXGAMEPLAYABILITIES_API UExAbilityProvider : public UInterface
{
	GENERATED_BODY()
};

class EXGAMEPLAYABILITIES_API IExAbilityProvider
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void CollectAbilitCases(TArray<FExAbilityCase>& AbilityConfsToAppend) const = 0;

	UFUNCTION()
	virtual FAbilityProviderInfo GetProviderInfo();
};
