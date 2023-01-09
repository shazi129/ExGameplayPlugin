#pragma once

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "PawnStateSettings.generated.h"


USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FStreamingLevelState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UWorld> MainWorld;

	UPROPERTY(EditAnywhere)
		TMap<FName, UPawnStateAsset*> LevelState;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FPawnStateAssets
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TArray<UPawnStateAsset*> Assets;
};

UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "PawnState"))
class EXGAMEPLAYPLUGIN_API UPawnStateSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPawnStateSettings()
	{
		CategoryName = "Extension Gameplay";
	}

	UPROPERTY(Config, EditAnywhere, Category = "StreamingLevel")
		TArray<FStreamingLevelState> SteamingLevelState;

	UPROPERTY(Config, EditAnywhere, Category = "Basic")
		FPawnStateAssets GlobalPawnStateAssets;
};