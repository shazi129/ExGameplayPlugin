// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "Engine/World.h"
#include "GameFeatureAction_AddLevelInstances.generated.h"

class UGameInstance;
struct FComponentRequestHandle;
struct FWorldContext;
class ULevelStreamingDynamic;

// Description of a level to add to the world when this game feature is enabled
USTRUCT()
struct FGameFeatureLevelInstanceEntry
{
	GENERATED_BODY()

	// The level instance to dynamically load at runtime.
	UPROPERTY(EditAnywhere, Category="Instance Info")
	TSoftObjectPtr<UWorld> Level;

	// Specific world to load into. If left null, this level will be loaded for all worlds.
	UPROPERTY(EditAnywhere, Category = "Instance Info")
	TSoftObjectPtr<UWorld> TargetWorld;

	// The translational offset for this level instance.
	UPROPERTY(EditAnywhere, Category="Instance Info")
	FVector Location;
	
	// The rotational tranform for this level instance. 
	UPROPERTY(EditAnywhere, Category="Instance Info")
	FRotator Rotation;
};	

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddLevelInstances

/**
 * Loads specified level instances at runtime.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Level Instances"))
class UGameFeatureAction_AddLevelInstances final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating() override;
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

	/** List of levels to dynamically load when this game feature is enabled */
	UPROPERTY(EditAnywhere, Category="Level Instances", meta=(TitleProperty="Level", ShowOnlyInnerProperties))
	TArray<FGameFeatureLevelInstanceEntry> LevelInstanceList;

private:
	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	
	ULevelStreamingDynamic* LoadDynamicLevelForEntry(const FGameFeatureLevelInstanceEntry& Entry, UWorld* TargetWorld);	

	UFUNCTION() // UFunction so we can bind to a dynamic delegate
	void OnLevelLoaded();

	void DestroyAddedLevels();
	void CleanUpAddedLevel(ULevelStreamingDynamic* Level);

private:
	UPROPERTY(transient)
	TArray<ULevelStreamingDynamic*> AddedLevels;

	bool bIsActivated = false;
	bool bLayerStateReentrantGuard = false;
};
