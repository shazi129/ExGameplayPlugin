#include "ExGameFeaturesSubsystem.h"
#include "ExGameFeaturesModule.h"
#include "GameFeaturesSubsystem.h"
#include "ExGameFeaturesCheat.h"
#include "Components/GameFrameworkComponentManager.h"

void UExGameFeaturesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency(UGameFrameworkComponentManager::StaticClass());

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

	ModularActionsMap.Empty();
	ModularActionAssetMap.Empty();

	Super::Deinitialize();
}

bool UExGameFeaturesSubsystem::AddModularActions(const FGameplayTag& ModularTag, const FModularActions& ModularActions)
{
	if (ModularActionsMap.Contains(ModularTag))
	{
		EXIGAMEFEATURE_LOG(Warning, TEXT("%s warning: %s already exist"), *FString(__FUNCTION__), *ModularTag.ToString())
		return false;
	}

	EXIGAMEFEATURE_LOG(Log, TEXT("%s: %s, ActivateDefault:%d"), *FString(__FUNCTION__), *ModularTag.ToString(), ModularActions.ActivateDefault);

	//填充Instance
	FModularActionsInstance& Instance = ModularActionsMap.Add(ModularTag);
	for (auto Action : ModularActions.Actions)
	{
		Instance.ModularActrions.Add(Action);
	}
	Instance.IsActivated = false;

	//如果是默认需要激活
	if (ModularActions.ActivateDefault)
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

	EXIGAMEFEATURE_LOG(Log, TEXT("%s: %s"), *FString(__FUNCTION__), *ModularTag.ToString());
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
		EXIGAMEFEATURE_LOG(Error, TEXT("%s error: No ModuleTag[%s]"), *FString(__FUNCTION__), *ModularTag.ToString());
		return false;
	}

	EXIGAMEFEATURE_LOG(Log, TEXT("%s: %s, Action Num: %d"), *FString(__FUNCTION__), *ModularTag.ToString(), Instance->ModularActrions.Num());
	for (auto& GameFeatureAction : Instance->ModularActrions)
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
	
	EXIGAMEFEATURE_LOG(Log, TEXT("%s: %s"), *FString(__FUNCTION__), *ModularTag.ToString());
	for (auto& GameFeatureAction : Instance->ModularActrions)
	{
		if (!GameFeatureAction.IsNull() && GameFeatureAction.IsValid())
		{
			FGameFeatureDeactivatingContext Context(nullptr);
			GameFeatureAction->OnGameFeatureDeactivating(Context);
		}
	}
	Instance->IsActivated = false;

	return true;
}

bool UExGameFeaturesSubsystem::LoadModuarActionData(TSoftObjectPtr<UModularActionsAssetData> ModularActionAssetData)
{
	if (!ModularActionAssetData.IsNull())
	{
		FString AssetPackageName = ModularActionAssetData.GetLongPackageName();

		//已经加载过了
		if (ModularActionAssetMap.Contains(AssetPackageName))
		{
			EXIGAMEFEATURE_LOG(Warning, TEXT("%s warning: ignore duplicate asset %s"), *FString(__FUNCTION__), *AssetPackageName);
			return false;
		}

		//加载资源
		UModularActionsAssetData* ActionsDataPtr = ModularActionAssetData.LoadSynchronous();
		if (!ActionsDataPtr)
		{
			EXIGAMEFEATURE_LOG(Error, TEXT("%s error: cannot load %s"), *FString(__FUNCTION__), *ModularActionAssetData.ToString());
			return false;
		}

		//保存，防止资源被释放
		ModularActionAssetMap.Add(AssetPackageName, ActionsDataPtr);

		//建立tag -> Action的索引
		for (const auto& ModularActionData : ActionsDataPtr->ModularActionsMap)
		{
			AddModularActions(ModularActionData.Key, ModularActionData.Value);
		}
		return true;
	}
	return false;
}

bool UExGameFeaturesSubsystem::LoadDefaultModularActions()
{
	auto& ActionsDataConfigs = GetMutableDefault<UExGameFeaturesSettings>()->ModularActionDatas;
	for (auto& ActionsDataConfig : ActionsDataConfigs)
	{
		LoadModuarActionData(ActionsDataConfig);
	}
	return true;
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
	FString Content = FString::Printf(TEXT("Activate: %d\n\tActions:\n"), IsActivated);
	for (auto Action : ModularActrions)
	{
		if (Action.IsNull() || !Action.IsValid())
		{
			Content += FString::Printf(TEXT("\t\tNULL\n"));
		}
		else
		{
			Content += FString::Printf(TEXT("\t\t%s\n"), *Action.ToString());
		}
	}
	return Content;
}
