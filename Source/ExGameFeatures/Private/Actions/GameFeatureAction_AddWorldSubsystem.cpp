#include "Actions/GameFeatureAction_AddWorldSubsystem.h"
#include "Engine/AssetManager.h"
#include "ExGameFeaturesModule.h"
#include "ExGameplayLibrary.h"
#include "GameFeaturesSubsystemSettings.h"

#define LOCTEXT_NAMESPACE "ExGameFeatures"

void UGameFeatureAction_AddWorldSubsystem::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	ClearSubystem(true);
}

#if WITH_EDITORONLY_DATA
void UGameFeatureAction_AddWorldSubsystem::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	if (UAssetManager::IsValid())
	{
		for (const FAddWorldSubsystemEntries& Entries : SubystemEntriesList)
		{
			for (const FAddWorldSubsystemEntry& Subsystem : Entries.Subsystems)
			{
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, Subsystem.SubsystemClass->GetPathName());
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateServer, Subsystem.SubsystemClass->GetPathName());
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
	for (const FAddWorldSubsystemEntries& Entries : SubystemEntriesList)
	{
		int32 SubsystemIndex = 0;
		for (const FAddWorldSubsystemEntry& Subsystem : Entries.Subsystems)
		{
			if (Subsystem.SubsystemClass == nullptr)
			{
				ValidationErrors.Add(FText::Format(LOCTEXT("NullSpawnedSubsystemType", "Null SubsystemType for Subsystem #{0} at index {1} in SubystemEntryList."), FText::AsNumber(SubsystemIndex), FText::AsNumber(EntryIndex)));
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
		const bool bIsServer = NetMode == NM_DedicatedServer || NetMode == NM_ListenServer;
		const bool bIsClient = !bIsServer;

		EXIGAMEFEATURE_LOG(Log, TEXT("%s start, Subsystem entry num: %d, NetMode:%d (client: %d, server: %d)"), *FString(__FUNCTION__), SubystemEntriesList.Num(), NetMode, bIsClient ? 1 : 0, bIsServer ? 1 : 0);
	
		for (auto& SubsystemEntries : SubystemEntriesList)
		{
			if (!SubsystemEntries.TargetWorld.IsNull())
			{
				FString TargetWorldPath = SubsystemEntries.TargetWorld.GetLongPackageName();
				FString CurrentWorldPath = UExGameplayLibrary::GetPackageFullName(World);
				if (TargetWorldPath != CurrentWorldPath)
				{
					UE_LOG(LogTemp, Log, TEXT("%s ignore, Current World[%s],  Target World[%s]"), *FString(__FUNCTION__), *CurrentWorldPath, *TargetWorldPath);
					continue;
				}
			}

			for (auto& SubsystemEntry : SubsystemEntries.Subsystems)
			{
				bool ShouldAdd = (bIsServer && SubsystemEntry.bInServer) || (bIsClient && SubsystemEntry.bInClient);
				EXIGAMEFEATURE_LOG(Log, TEXT("%s: %s --> %s, ShouldAdd:%d"), *FString(__FUNCTION__), *GetNameSafe(SubsystemEntry.SubsystemClass), *GetNameSafe(World), ShouldAdd);
				if (!ShouldAdd)
				{
					continue;
				}

				if (SubsystemEntry.SubsystemClass == nullptr)
				{
					EXIGAMEFEATURE_LOG(Error, TEXT("%s error, Subsystem is null"), *FString(__FUNCTION__));
					continue;
				}

				if (AddedSubsystems.Contains(SubsystemEntry.SubsystemClass) || World->GetSubsystemBase(SubsystemEntry.SubsystemClass))
				{
					EXIGAMEFEATURE_LOG(Log, TEXT("%s Ignore, Subsystem %s already exist"), *FString(__FUNCTION__), *GetNameSafe(SubsystemEntry.SubsystemClass));
					continue;
				}

				if (UModularWorldSubsystem* CDO = SubsystemEntry.SubsystemClass->GetDefaultObject<UModularWorldSubsystem>())
				{
					CDO->bShouldCreate = CDO->NativeCanActivate(World);
					if (CDO->ShouldCreateSubsystem(World))
					{
						FSubsystemCollectionBase::ActivateExternalSubsystem(SubsystemEntry.SubsystemClass);
						AddedSubsystems.Add(SubsystemEntry.SubsystemClass, &SubsystemEntry);
					}
				}
			}
		}
		
	}
}

void UGameFeatureAction_AddWorldSubsystem::RemoveFromWorld(const UWorld* World)
{
	ClearSubystem(false);
}

void UGameFeatureAction_AddWorldSubsystem::ClearSubystem(bool ClearAll)
{
	TArray<TSubclassOf<UModularWorldSubsystem>> DeactivateSubsystems;
	for (auto& AddedSubsystemItem : AddedSubsystems)
	{
		if (ClearAll || !AddedSubsystemItem.Value || AddedSubsystemItem.Value->bDeactivateWhenWorldTeardown)
		{
			DeactivateSubsystems.Add(AddedSubsystemItem.Key);
		}
	}

	for (auto& SubsystemClass : DeactivateSubsystems)
	{
		AddedSubsystems.Remove(SubsystemClass);
		if (UModularWorldSubsystem* CDO = SubsystemClass->GetDefaultObject<UModularWorldSubsystem>())
		{
			CDO->bShouldCreate = false;
			FSubsystemCollectionBase::DeactivateExternalSubsystem(SubsystemClass);
		}
	}
}


#undef LOCTEXT_NAMESPACE
