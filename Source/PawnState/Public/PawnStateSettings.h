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
		FGameplayTag WorldPawnState;

	UPROPERTY()
		UPawnStateAsset* WorldState;

	UPROPERTY()
		TMap<FName, UPawnStateAsset*> LevelState;
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
		TArray<TSoftObjectPtr<UPawnStateAssets>> GlobalPawnStateAssets;
};