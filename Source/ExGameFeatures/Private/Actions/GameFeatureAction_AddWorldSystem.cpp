// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/GameFeatureAction_AddWorldSystem.h"
#include "Engine/AssetManager.h"
#include "GameFeaturesSubsystemSettings.h"

#define LOCTEXT_NAMESPACE "AncientGameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddWorldSystem

void UGameFeatureAction_AddWorldSystem::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	Reset();
}

#if WITH_EDITORONLY_DATA
void UGameFeatureAction_AddWorldSystem::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	if (UAssetManager::IsValid())
	{
		for (const FGameFeatureWorldSystemEntry& Entry : WorldSystemsList)
		{
			if (Entry.SystemType)
			{
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, Entry.SystemType->GetPathName());
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateServer, Entry.SystemType->GetPathName());
			}
		}
	}
}
#endif

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddWorldSystem::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FGameFeatureWorldSystemEntry& Entry : WorldSystemsList)
	{
		if (Entry.SystemType == nullptr)
		{
			Result = EDataValidationResult::Invalid;
			ValidationErrors.Add(FText::Format(LOCTEXT("NullWorldSystemType", "Null SystemType at index {0} in WorldSystemsList."), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddWorldSystem::AddToWorld(const FWorldContext& WorldContext)
{
	UWorld* World = WorldContext.World();

	if ((World != nullptr) && World->IsGameWorld())
	{
		UGameFeaturesWorldManager* SystemManager = World->GetSubsystem<UGameFeaturesWorldManager>();
		if (ensure(SystemManager))
		{
			for (const FGameFeatureWorldSystemEntry& Entry : WorldSystemsList)
			{
				if (!Entry.TargetWorld.IsNull())
				{
					UWorld* TargetWorld = Entry.TargetWorld.Get();
					if (TargetWorld && TargetWorld->GetName() != World->GetName())
					{
						// This system is intended for a specific world (not this one)
						UE_LOG(LogTemp, Log, TEXT("UGameFeatureAction_AddWorldSystem.AddToWorld ignore, Current World[%s],  Target World[%s]"), *(World->GetName()), *(TargetWorld->GetName()));
						continue;
					}
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("UGameFeatureAction_AddWorldSystem.AddToWorld, add to any world"));
				}

				if (Entry.SystemType)
				{
					UE_LOG(LogTemp, Log, TEXT("UGameFeatureAction_AddWorldSystem.AddToWorld, World[%s] NetMode[%d]"), *(World->GetName()), World->GetNetMode());
					SystemManager->RequestSystemOfType(Entry.SystemType);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UGameFeatureAction_AddWorldSystem.AddToWorld ignore, SystemType is Null[%d]"), (Entry.SystemType == nullptr));
				}
			}
		}
	}
}

void UGameFeatureAction_AddWorldSystem::Reset()
{
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		if (World)
		{
			if (UGameFeaturesWorldManager* SystemManager = World->GetSubsystem<UGameFeaturesWorldManager>())
			{
				for (const FGameFeatureWorldSystemEntry& Entry : WorldSystemsList)
				{
					if (!Entry.TargetWorld.IsNull())
					{
						UWorld* TargetWorld = Entry.TargetWorld.Get();
						if (TargetWorld != World)
						{
							// This system is intended for a specific world (not this one)
							continue;
						}
					}

					if (Entry.SystemType)
					{
						SystemManager->ReleaseRequestForSystemOfType(Entry.SystemType);
					}
				}

			}
		}
	}
}


#undef LOCTEXT_NAMESPACE