// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddSpawnedActors.generated.h"

/** Record for the an actor to spawn along with a game feature data. */
USTRUCT()
struct FSpawningActorEntry
{
	GENERATED_BODY()

	// What kind of actor to spawn
	UPROPERTY(EditAnywhere, Category = "Actor")
	TSubclassOf<AActor> ActorType;

	// Where to spawn the actor
	UPROPERTY(EditAnywhere, Category = "Actor|Transform")
	FTransform SpawnTransform;

	// Should this actor be added for clients
	UPROPERTY(EditAnywhere, Category = "Actor")
		uint8 bClientActor : 1;

	// Should this actor be added on servers
	UPROPERTY(EditAnywhere, Category = "Actor")
		uint8 bServerActor : 1;
};

/** Record for the game feature data. Specifies which actors to spawn for target worlds. */
USTRUCT()
struct FSpawningWorldActorsEntry
{
	GENERATED_BODY()

	// The world to spawn the actors for (can be left blank, in which case we'll spawn them for all worlds)
	UPROPERTY(EditAnywhere, Category="Feature Data")
	TSoftObjectPtr<UWorld> TargetWorld;

	// The actors to spawn
	UPROPERTY(EditAnywhere, Category = "Feature Data")
	TArray<FSpawningActorEntry> Actors;
};	

/**
 * GameFeature action which will spawn actors into particular levels at runtime.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Spawned Actors"))
class UGameFeatureAction_AddSpawnedActors final : public UGameFeatureAction_WorldActionBase
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

	UPROPERTY(EditAnywhere, Category="Actor")
	TArray<FSpawningWorldActorsEntry> ActorsList;

private:
	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	void Reset();

	TArray<TWeakObjectPtr<AActor>> SpawnedActors;
};
