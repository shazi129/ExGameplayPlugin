#include "Actions/GameFeatureAction_AddWorldSubsystem.h"
#include "Engine/AssetManager.h"
#include "GameFeaturesSubsystemSettings.h"
#include "ExGameFeaturesModule.h"
#include "ExGameplayLibrary.h"

#define LOCTEXT_NAMESPACE "ExGameFeatures"

void UGameFeatureAction_AddWorldSubsystem::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

#if WITH_EDITORONLY_DATA
void UGameFeatureAction_AddWorldSubsystem::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	if (UAssetManager::IsValid())
	{
		for (const FAddWorldSubsystemEntries& Entry : EntriesList)
		{
			for (const FAddWorldSubsystemEntry& SubsystemEntry : Entry.Subsystems)
			{
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, SubsystemEntry.SubsystemClass.ToSoftObjectPath().GetAssetPath());
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateServer, SubsystemEntry.SubsystemClass.ToSoftObjectPath().GetAssetPath());
			}
		}
	}
}
#endif

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddWorldSubsystem::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FAddWorldSubsystemEntries& Entry : EntriesList)
	{
		int32 SubsystemIndex = 0;
		for (const FAddWorldSubsystemEntry& SubsystemEntry : Entry.Subsystems)
		{
			if (SubsystemEntry.SubsystemClass == nullptr)
			{
				ValidationErrors.Add(FText::Format(LOCTEXT("NullSpawnedActorType", "Null ActorType for actor #{0} at index {1} in ActorsList."), FText::AsNumber(SubsystemIndex), FText::AsNumber(EntryIndex)));
			}
			++SubsystemIndex;
		}
		++EntryIndex;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddWorldSubsystem::AddToWorld(const FWorldContext& WorldContext)
{
	UWorld* World = WorldContext.World();

	if ((World != nullptr) && World->IsGameWorld())
	{
		const ENetMode NetMode = World->GetNetMode();
		const bool bIsServer = NetMode != NM_Client;
		const bool bIsClient = NetMode != NM_DedicatedServer;

		for (const FAddWorldSubsystemEntries& EntriesItem : EntriesList)
		{
			if (!EntriesItem.TargetWorld.IsNull())
			{
				FString TargetWorldPath = EntriesItem.TargetWorld.GetLongPackageName();
				FString CurrentWorldPath = UExGameplayLibrary::GetPackageFullName(World);
				if (TargetWorldPath != CurrentWorldPath)
				{
					// This system is intended for a specific world (not this one)
					EXIGAMEFEATURE_LOG(Log, TEXT("%s ignore, Current World[%s],  Target World[%s]"), *FString(__FUNCTION__), *CurrentWorldPath, *TargetWorldPath);
					continue;
				}
			}

			for (const FAddWorldSubsystemEntry& SubsystemEntry : EntriesItem.Subsystems)
			{
				bool ShouldSpawn = (bIsServer && SubsystemEntry.bInServer) || (bIsClient && SubsystemEntry.bInClient);
				if (ShouldSpawn && SubsystemEntry.SubsystemClass != nullptr)
				{
				}
				else
				{
					EXIGAMEFEATURE_LOG(Error, TEXT("%s ignore, ShouldSpawn[%d], Actor is Null[%d]"),
						*FString(__FUNCTION__), ShouldSpawn, (SubsystemEntry.SubsystemClass == nullptr));
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE