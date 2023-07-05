
#pragma once

#include "Actions/GameFeatureAction_WorldActionBase.h"
#include "GameFeaturesSubsystem.h"
#include "Modular/ModularWorldSubsystem.h"
#include "GameFeatureAction_AddWorldSubsystem.generated.h"

USTRUCT()
struct FAddWorldSubsystemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TSoftClassPtr<UModularWorldSubsystem> SubsystemClass;

	UPROPERTY(EditAnywhere, Category = "Components")
		uint8 bInClient : 1;

	UPROPERTY(EditAnywhere, Category = "Components")
		uint8 bInServer : 1;

	FAddWorldSubsystemEntry()
		: bInClient(true)
		, bInServer(true)
	{
	}
};
/*
USTRUCT()
struct FAddWorldSubsystemEntries
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UWorld> TargetWorld;

	UPROPERTY(EditAnywhere)
		TArray<FAddWorldSubsystemEntry> Subsystems;
};


UCLASS(MinimalAPI, meta = (DisplayName = "Add World Subsystem"))
class UGameFeatureAction_AddWorldSubsystem final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
#endif
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif
	//~ End UObject interface

public:
	UPROPERTY(EditAnywhere)
		TArray<FAddWorldSubsystemEntries> EntriesList;

private:
	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface
};
*/
