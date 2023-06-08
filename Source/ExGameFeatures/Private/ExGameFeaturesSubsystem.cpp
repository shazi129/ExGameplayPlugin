#include "ExGameFeaturesSubsystem.h"
#include "ExGameFeaturesModule.h"
#include "GameFeaturesSubsystem.h"
#include "ExGameFeaturesCheat.h"

void UExGameFeaturesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadDefaultModularActions();

#if UE_WITH_CHEAT_MANAGER
	CheatCreateHandle = UCheatManager::RegisterForOnCheatManagerCreated(
		FOnCheatManagerCreated::FDelegate::CreateUObject(this, &UExGameFeaturesSubsystem::OnCheatCreate));
#endif
}

void UExGameFeaturesSubsystem::Deinitialize()
{
	if (CheatCreateHandle.IsValid())
	{
		UCheatManager::UnregisterFromOnCheatManagerCreated(CheatCreateHandle);
	}

	//析构所有Action
	for (auto& ModularActionsItem : ModularActionsMap)
	{
		DeactivateModule(ModularActionsItem.Key);
	}

	Super::Deinitialize();
}

bool UExGameFeaturesSubsystem::AddModularActions(const FGameplayTag& ModularTag, const FModularActions& ModularActions)
{
	if (ModularActionsMap.Contains(ModularTag))
	{
		EXIGAMEFEATURE_LOG(Warning, TEXT("%s warning: %s already exist"), *FString(__FUNCTION__), *ModularTag.ToString())
		return false;
	}

	FModularActionsInstance& Instance = ModularActionsMap.Add(ModularTag);
	Instance.ModularActrions = ModularActions;
	if (Instance.ModularActrions.ActivateDefault)
	{
		ActivateModule(ModularTag);
	}

	return true;
}

bool UExGameFeaturesSubsystem::RemoveModularActions(const FGameplayTag& ModularTag)
{
	FModularActionsInstance* Instance = ModularActionsMap.Find(ModularTag);
	if (!Instance)
	{
		return true;
	}

	if (Instance->IsActivated)
	{
		ActivateModule(ModularTag);
	}

	ModularActionsMap.Remove(ModularTag);

	return true;
}

bool UExGameFeaturesSubsystem::ActivateModule(const FGameplayTag& ModularTag)
{
	FModularActionsInstance* Instance = ModularActionsMap.Find(ModularTag);
	if (!Instance)
	{
		EXIGAMEFEATURE_LOG(Error, TEXT("%s error: No ModuleTag[%s]"));
		return false;
	}

	for (auto& GameFeatureAction : Instance->ModularActrions.Actions)
	{
		if (GameFeatureAction)
		{
			FGameFeatureActivatingContext Context;
			GameFeatureAction->OnGameFeatureActivating(Context);
		}
	}
	Instance->IsActivated = true;

	return true;
}

bool UExGameFeaturesSubsystem::DeactivateModule(const FGameplayTag& ModularTag)
{
	FModularActionsInstance* Instance = ModularActionsMap.Find(ModularTag);
	if (!Instance)
	{
		return false;
	}
	
	for (auto& GameFeatureAction : Instance->ModularActrions.Actions)
	{
		if (GameFeatureAction)
		{
			FGameFeatureDeactivatingContext Context(nullptr);
			GameFeatureAction->OnGameFeatureDeactivating(Context);
		}
	}
	Instance->IsActivated = false;

	return true;
}

void UExGameFeaturesSubsystem::LoadDefaultModularActions()
{
	TSoftObjectPtr<UModularActionData>& ActionsDataConfig = GetMutableDefault<UExGameFeaturesSettings>()->ModularActionData;
	if (!ActionsDataConfig.IsNull())
	{
		UModularActionData* ActionsDataPtr = ActionsDataConfig.LoadSynchronous();

		for (const auto& ModularActionData : ActionsDataPtr->ModularActionsMap)
		{
			AddModularActions(ModularActionData.Key, ModularActionData.Value);
		}
	}
}

void UExGameFeaturesSubsystem::OnCheatCreate(UCheatManager* CheatManager)
{
	if (CheatManager)
	{
		CheatManager->AddCheatManagerExtension(NewObject<UExGameFeaturesCheat>(CheatManager, UExGameFeaturesCheat::StaticClass()));
	}
}

FString FModularActionsInstance::ToString() const
{
	FString Content = FString::Printf(TEXT("Activate: %d"), IsActivated);
	return Content;
}
