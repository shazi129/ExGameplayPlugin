#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Modular/ModularActionData.h"
#include "ExGameFeaturesSettings.generated.h"

UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "Gameplay Extension"))
class EXGAMEFEATURES_API UExGameFeaturesSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UExGameFeaturesSettings()
	{
		CategoryName = "Game";
	}

	UPROPERTY(Config, EditAnywhere, Category = "GameFeatures")
		TArray<FString> DefaultGameFeatures;

	UPROPERTY(Config, EditAnywhere, Category = "GameFeatures")
		TSoftObjectPtr<UModularActionData> ModularActionData;
};