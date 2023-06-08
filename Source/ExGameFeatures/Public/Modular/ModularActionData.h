#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "ModularActionData.generated.h"


USTRUCT(BlueprintType)
struct EXGAMEFEATURES_API FModularActions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	UPROPERTY(EditAnywhere)
	bool ActivateDefault = false;
};

UCLASS()
class EXGAMEFEATURES_API UModularActionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, FModularActions> ModularActionsMap;
};
