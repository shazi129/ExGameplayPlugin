// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/GameFeatureAction_AddLevelInstances.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFeaturesSubsystemSettings.h"
#include "Engine/AssetManager.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogAncientGameFeatures, Log, All);
#define LOCTEXT_NAMESPACE "AncientGameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddLevelInstances

void UGameFeatureAction_AddLevelInstances::OnGameFeatureActivating()
{
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UGameFeatureAction_AddLevelInstances::OnWorldCleanup);

	if (!ensureAlways(AddedLevels.Num() == 0))
	{
		DestroyAddedLevels();
	}

	bIsActivated = true;
	Super::OnGameFeatureActivating();
}

void UGameFeatureAction_AddLevelInstances::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	DestroyAddedLevels();
	bIsActivated = false;

	FWorldDelegates::OnWorldCleanup.RemoveAll(this);
	Super::OnGameFeatureDeactivating(Context);
}

#if WITH_EDITORONLY_DATA
void UGameFeatureAction_AddLevelInstances::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	if (UAssetManager::IsValid())
	{
		for (const FGameFeatureLevelInstanceEntry& Entry : LevelInstanceList)
		{
			AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, Entry.Level.ToSoftObjectPath());
			AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateServer, Entry.Level.ToSoftObjectPath());
		}
	}
}
#endif

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddLevelInstances::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FGameFeatureLevelInstanceEntry& Entry : LevelInstanceList)
	{
		if (Entry.Level.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			ValidationErrors.Add(FText::Format(LOCTEXT("LevelEntryNull", "Null level reference at index {0} in LevelInstanceList"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddLevelInstances::AddToWorld(const FWorldContext& WorldContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	if (ensureAlways(bIsActivated) && (GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		AddedLevels.Reserve(AddedLevels.Num() + LevelInstanceList.Num());

		for (const FGameFeatureLevelInstanceEntry& Entry : LevelInstanceList)
		{
			if (!Entry.Level.IsNull())
			{
				if (!Entry.TargetWorld.IsNull())
				{
					UWorld* TargetWorld = Entry.TargetWorld.Get();
					if (TargetWorld != World)
					{
						// This level is intended for a specific world (not this one)
						continue;
					}
				}

				LoadDynamicLevelForEntry(Entry, World);
			}
		}
	}

	GEngine->BlockTillLevelStreamingCompleted(World);
}

void UGameFeatureAction_AddLevelInstances::OnWorldCleanup(UWorld* World, bool /*bSessionEnded*/, bool /*bCleanupResources*/)
{
	int32 FoundIndex = AddedLevels.IndexOfByPredicate([World](ULevelStreamingDynamic* InStreamingLevel)
		{
			return InStreamingLevel && InStreamingLevel->GetWorld() == World;
		});

	if (FoundIndex != INDEX_NONE)
	{
		CleanUpAddedLevel(AddedLevels[FoundIndex]);
		AddedLevels.RemoveAtSwap(FoundIndex);
	}
}

ULevelStreamingDynamic* UGameFeatureAction_AddLevelInstances::LoadDynamicLevelForEntry(const FGameFeatureLevelInstanceEntry& Entry, UWorld* TargetWorld)
{
	bool bSuccess = false;
	ULevelStreamingDynamic* StreamingLevelRef = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(TargetWorld, Entry.Level, Entry.Location, Entry.Rotation, bSuccess);

	if (!bSuccess)
	{
		UE_LOG(LogAncientGameFeatures, Error, TEXT("[GameFeatureData %s]: Failed to load level instance `%s`."), *GetPathNameSafe(this), *Entry.Level.ToString());
	}
	else if (StreamingLevelRef)
	{
		AddedLevels.Add(StreamingLevelRef);
	}

	return StreamingLevelRef;
}

void UGameFeatureAction_AddLevelInstances::OnLevelLoaded()
{
	if (ensureAlways(bIsActivated))
	{
		// We don't have a way of knowing which instance this was triggered for, so we have to look through them all...
		for (ULevelStreamingDynamic* Level : AddedLevels)
		{
			if (Level && Level->GetCurrentState() == ULevelStreaming::ECurrentState::LoadedNotVisible)
			{
				Level->SetShouldBeVisible(true);
			}
		}
	}
}

void UGameFeatureAction_AddLevelInstances::DestroyAddedLevels()
{
	for (ULevelStreamingDynamic* Level : AddedLevels)
	{
		CleanUpAddedLevel(Level);
	}
	AddedLevels.Empty();
}

void UGameFeatureAction_AddLevelInstances::CleanUpAddedLevel(ULevelStreamingDynamic* Level)
{
	if (Level)
	{
		Level->OnLevelLoaded.RemoveAll(this);
		Level->SetIsRequestingUnloadAndRemoval(true);
	}
}

//////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE