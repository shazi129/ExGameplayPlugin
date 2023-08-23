
#pragma once

#include "Actions/GameFeatureAction_WorldActionBase.h"
#include "GameFeaturesSubsystem.h"
#include "Modular/ModularWorldSubsystem.h"
#include "GameFeatureAction_AddWorldSubsystem.generated.h"

USTRUCT()
struct EXGAMEFEATURES_API FAddWorldSubsystemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TSubclassOf<UModularWorldSubsystem> SubsystemClass;

	UPROPERTY(EditAnywhere)
		uint8 bInClient : 1;

	UPROPERTY(EditAnywhere)
		uint8 bInServer : 1;

	UPROPERTY(EditAnywhere)
		uint8 bDeactivateWhenWorldTeardown : 1;

	FAddWorldSubsystemEntry()
		: bInClient(true)
		, bInServer(true)
		, bDeactivateWhenWorldTeardown(false)
	{
	}
};

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
		TArray<FAddWorldSubsystemEntries> SubystemEntriesList;

private:
	virtual void AddToWorld(const FWorldContext& WorldContext) override;
	virtual void RemoveFromWorld(const UWorld* World) override;

	void ClearSubystem(bool ClearAll=false);

	TMap<TSubclassOf<UModularWorldSubsystem>, FAddWorldSubsystemEntry*> AddedSubsystems;
};

