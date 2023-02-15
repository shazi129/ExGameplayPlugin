#pragma once

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "PawnStateSettings.generated.h"


USTRUCT(BlueprintType)
struct PAWNSTATE_API FWorldPawnStateInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UWorld> MainWorld;

	UPROPERTY(EditAnywhere)
		UPawnStateAsset* WorldState;

	UPROPERTY(EditAnywhere)
		TMap<FName, UPawnStateAsset*> LevelState;
};

USTRUCT(BlueprintType)
struct PAWNSTATE_API FPawnStateAssets
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TArray<UPawnStateAsset*> Assets;
};

UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "PawnState"))
class PAWNSTATE_API UPawnStateSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPawnStateSettings()
	{
		CategoryName = "Extension Gameplay";
	}

	UPROPERTY(Config, EditAnywhere, Category = "World")
		TArray<FWorldPawnStateInfo> WorldPawnStates;

	UPROPERTY(Config, EditAnywhere, Category = "Basic")
		FPawnStateAssets GlobalPawnStateAssets;
};