#include "Settings/ExGameplaySettingSubsystem.h"
#include "Settings/ExGameplaySettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFeaturesSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"
#include "GlobalVariableSubsystem.h"

void UExGameplaySettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency(UGlobalVariableSubsystem::StaticClass());

#if UE_WITH_CHEAT_MANAGER
	CheatManagerCreateHandle = UCheatManager::RegisterForOnCheatManagerCreated(
			FOnCheatManagerCreated::FDelegate::CreateUObject(this, &UExGameplaySettingSubsystem::OnCheatManagerCreate));
#endif

	InitGlobalVariables();
}

void UExGameplaySettingSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (CheatManagerCreateHandle.IsValid())
	{
		UCheatManager::UnregisterFromOnCheatManagerCreated(CheatManagerCreateHandle);
	}
}

void UExGameplaySettingSubsystem::OnCheatManagerCreate(UCheatManager* CheatManager)
{
	if (CheatManager)
	{
		auto CheatExtensionClasses = GetDefault<UExGameplaySettings>()->CheatExtensionClasses;
		for (const FSoftClassPath& ClassPath : CheatExtensionClasses)
		{
			UClass* ExtensionClass = LoadClass<UCheatManagerExtension>(nullptr, *ClassPath.ToString());
			if (!ExtensionClass)
			{
				UE_LOG(LogTemp, Error, TEXT("UGameplaySettingSubsystem::OnCheatManagerCreate error, cannot load class:%s"), *ClassPath.ToString());
				continue;
			}

			CheatManager->AddCheatManagerExtension(NewObject<UCheatManagerExtension>(CheatManager, ExtensionClass));
		}
	}
}

void UExGameplaySettingSubsystem::InitGlobalVariables()
{
	UGlobalVariableSubsystem* GlobalVariableSubsystem = UGlobalVariableSubsystem::GetSubsystem(this);
	for (auto& BoolItem : GetDefault<UExGameplaySettings>()->BoolVariables)
	{
		GlobalVariableSubsystem->SetBoolVariable(BoolItem.Key, BoolItem.Value);
	}

	for (auto& BoolItem : GetDefault<UExGameplaySettings>()->IntVariables)
	{
		GlobalVariableSubsystem->SetIntVariable(BoolItem.Key, BoolItem.Value);
	}
}
