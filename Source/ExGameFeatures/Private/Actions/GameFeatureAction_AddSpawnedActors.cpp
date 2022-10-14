// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/GameFeatureAction_AddSpawnedActors.h"
#include "Engine/AssetManager.h"
#include "GameFeaturesSubsystemSettings.h"

#define LOCTEXT_NAMESPACE "AncientGameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddWorldSystem

void UGameFeatureAction_AddSpawnedActors::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	Reset();
}

#if WITH_EDITORONLY_DATA
void UGameFeatureAction_AddSpawnedActors::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	if (UAssetManager::IsValid())
	{
		for (const FSpawningWorldActorsEntry& Entry : ActorsList)
		{
			for (const FSpawningActorEntry& ActorEntry : Entry.Actors)
			{
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, ActorEntry.ActorType->GetPathName());
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateServer, ActorEntry.ActorType->GetPathName());
			}
		}
	}
}
#endif

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddSpawnedActors::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FSpawningWorldActorsEntry& Entry : ActorsList)
	{
		int32 ActorIndex = 0;
		for (const FSpawningActorEntry& ActorEntry : Entry.Actors)
		{
			if (ActorEntry.ActorType == nullptr)
			{
				ValidationErrors.Add(FText::Format(LOCTEXT("NullSpawnedActorType", "Null ActorType for actor #{0} at index {1} in ActorsList."), FText::AsNumber(ActorIndex), FText::AsNumber(EntryIndex)));
			}
			++ActorIndex;
		}
		++EntryIndex;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddSpawnedActors::AddToWorld(const FWorldContext& WorldContext)
{
	UE_LOG(LogTemp, Log, TEXT("UGameFeatureAction_AddSpawnedActors.AddToWorld start"));
	UWorld* World = WorldContext.World();

	if ((World != nullptr) && World->IsGameWorld())
	{
		const ENetMode NetMode = World->GetNetMode();
		const bool bIsServer = NetMode != NM_Client;
		const bool bIsClient = NetMode != NM_DedicatedServer;
		UE_LOG(LogTemp, Verbose, TEXT("Adding Actor for %s to world %s (client: %d, server: %d)"), *GetPathNameSafe(this), *World->GetDebugDisplayName(), bIsClient ? 1 : 0, bIsServer ? 1 : 0);

		for (const FSpawningWorldActorsEntry& Entry : ActorsList)
		{
			if (!Entry.TargetWorld.IsNull())
			{
				UWorld* TargetWorld = Entry.TargetWorld.Get();
				if (TargetWorld && TargetWorld->GetName() != World->GetName())
				{
					// This system is intended for a specific world (not this one)
					UE_LOG(LogTemp, Log, TEXT("UGameFeatureAction_AddSpawnedActors.AddToWorld ignore, Current World[%s],  Target World[%s]"), *(World->GetName()), *(TargetWorld->GetName()));
					continue;
				}
			}

			for (const FSpawningActorEntry& ActorEntry : Entry.Actors)
			{
				bool ShouldSpawn = (bIsServer && ActorEntry.bServerActor) || (bIsClient && ActorEntry.bClientActor);
				if (ShouldSpawn && ActorEntry.ActorType != nullptr)
				{
					AActor* NewActor = World->SpawnActor<AActor>(ActorEntry.ActorType, ActorEntry.SpawnTransform);
					SpawnedActors.Add(NewActor);

					UE_LOG(LogTemp, Log, TEXT("UGameFeatureAction_AddSpawnedActors.AddToWorld, Actor[%s], Instance[%s], World[%s]"),
						*(NewActor->GetName()), *(WorldContext.OwningGameInstance->GetName()), *(World->GetName()));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UGameFeatureAction_AddSpawnedActors.AddToWorld ignore, ShouldSpawn[%d], Actor is Null[%d]"), ShouldSpawn, (ActorEntry.ActorType == nullptr));
				}
			}
		}
	}
}

void UGameFeatureAction_AddSpawnedActors::Reset()
{
	for (TWeakObjectPtr<AActor>& ActorPtr : SpawnedActors)
	{
		if (ActorPtr.IsValid())
		{
			ActorPtr->Destroy();
		}
	}
}

#undef LOCTEXT_NAMESPACE