#include "Settings/ExGameplaySettingSubsystem.h"
#include "Settings/ExGameplaySettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFeaturesSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"


void UExGameplaySettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if UE_WITH_CHEAT_MANAGER
	CheatManagerCreateHandle = UCheatManager::RegisterForOnCheatManagerCreated(
			FOnCheatManagerCreated::FDelegate::CreateUObject(this, &UExGameplaySettingSubsystem::OnCheatManagerCreate));
#endif
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
