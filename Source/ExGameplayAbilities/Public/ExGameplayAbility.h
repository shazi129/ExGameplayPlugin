#pragma once

#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "ExGameplayAbility.generated.h"


UCLASS(ClassGroup = (ExAbility), BlueprintType, Blueprintable, abstract, editinlinenew)
class EXGAMEPLAYABILITIES_API UExGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
};


USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FExAbilityCase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		TSubclassOf<UExGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		int AbilityLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		bool ActivateWhenGiven = false;

	bool IsValid() const
	{
		return AbilityClass != nullptr;
	}
};