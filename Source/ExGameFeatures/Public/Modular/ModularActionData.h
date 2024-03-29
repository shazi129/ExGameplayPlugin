#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameplayTags.h"
#include "ModularActionData.generated.h"


USTRUCT(BlueprintType)
struct EXGAMEFEATURES_API FModularActions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced)
	TArray<UGameFeatureAction*> Actions;

	UPROPERTY(EditAnywhere)
	bool ActivateDefault = false;
};

UCLASS()
class EXGAMEFEATURES_API UModularActionsAssetData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, FModularActions> ModularActionsMap;
};
