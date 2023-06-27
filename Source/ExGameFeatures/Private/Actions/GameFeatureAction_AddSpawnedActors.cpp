// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/GameFeatureAction_AddSpawnedActors.h"
#include "Engine/AssetManager.h"
#include "GameFeaturesSubsystemSettings.h"
#include "ExGameplayLibrary.h"
#include "ExGameFeaturesModule.h"

#define LOCTEXT_NAMESPACE "ExGameFeatures"

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
	UWorld* World = WorldContext.World();

	if ((World != nullptr) && World->IsGameWorld())
	{
		const ENetMode NetMode = World->GetNetMode();
		const bool bIsServer = NetMode == NM_DedicatedServer || NetMode == NM_ListenServer;
		const bool bIsClient = !bIsServer;

		for (const FSpawningWorldActorsEntry& Entry : ActorsList)
		{
			if (!Entry.TargetWorld.IsNull())
			{
				FString TargetWorldPath = Entry.TargetWorld.GetLongPackageName();
				FString CurrentWorldPath = UExGameplayLibrary::GetPackageFullName(World);
				if (TargetWorldPath != CurrentWorldPath)
				{
					// This system is intended for a specific world (not this one)
					UE_LOG(LogTemp, Log, TEXT("%s ignore, Current World[%s],  Target World[%s]"), *FString(__FUNCTION__), *CurrentWorldPath, *TargetWorldPath);
					continue;
				}
			}

			for (const FSpawningActorEntry& ActorEntry : Entry.Actors)
			{
				EXIGAMEFEATURE_LOG(Log, TEXT("%s: %s --> %s, NetMode:%d (client: %d, server: %d)"), *FString(__FUNCTION__), *GetNameSafe(ActorEntry.ActorType), *GetNameSafe(World), NetMode, bIsClient ? 1 : 0, bIsServer ? 1 : 0);

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