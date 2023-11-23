#include "Settings/GameplayUtilSettingSubsystem.h"
#include "Settings/GameplayUtilSettings.h"
#include "Kismet/GameplayStatics.h"

void UGameplayUtilSettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if UE_WITH_CHEAT_MANAGER
	CheatManagerCreateHandle = UCheatManager::RegisterForOnCheatManagerCreated(
		FOnCheatManagerCreated::FDelegate::CreateUObject(this, &UGameplayUtilSettingSubsystem::OnCheatManagerCreate));
#endif

	InitGlobalVariables();

	for (auto& ObjectPtr : GetDefault<UGameplayUtilSettings>()->PermanentObjects)
	{
		if (!ObjectPtr.IsNull())
		{
			PermanentObjects.Add(ObjectPtr.LoadSynchronous());
		}
	}
}

void UGameplayUtilSettingSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (CheatManagerCreateHandle.IsValid())
	{
		UCheatManager::UnregisterFromOnCheatManagerCreated(CheatManagerCreateHandle);
	}
}

void UGameplayUtilSettingSubsystem::OnCheatManagerCreate(UCheatManager* CheatManager)
{
	if (CheatManager)
	{
		auto CheatExtensionClasses = GetDefault<UGameplayUtilSettings>()->CheatExtensionClasses;
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

void UGameplayUtilSettingSubsystem::InitGlobalVariables()
{
}
