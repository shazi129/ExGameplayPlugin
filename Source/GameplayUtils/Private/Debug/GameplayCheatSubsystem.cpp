#include "Cheat/GameplayCheatSubsystem.h"
#include "Cheat/GameplayCheatExtension.h"
#include "Settings/GameplayUtilSettings.h"

void UGameplayCheatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if UE_WITH_CHEAT_MANAGER
	CheatCreateHandle = UCheatManager::RegisterForOnCheatManagerCreated(
		FOnCheatManagerCreated::FDelegate::CreateUObject(this, &UGameplayCheatSubsystem::OnCheatManagerCreate));
#endif
}

void UGameplayCheatSubsystem::Deinitialize()
{
	Super::Deinitialize();
	if (CheatCreateHandle.IsValid())
	{
		UCheatManager::UnregisterFromOnCheatManagerCreated(CheatCreateHandle);
	}
}

void UGameplayCheatSubsystem::OnCheatManagerCreate(UCheatManager* CheatManager)
{
	if (CheatManager)
	{
		//找定义的所有C++子类
		TArray<UClass*> CheatClasses;
		GetDerivedClasses(UGameplayCheatExtension::StaticClass(), CheatClasses, true);

		//找配置的类
		const auto& SettingsCheatClasses = GetDefault<UGameplayUtilSettings>()->CheatExtensionClasses;
		for (const auto& SettingsCheatClasse : SettingsCheatClasses)
		{
			if (UClass* Class = SettingsCheatClasse.LoadSynchronous())
			{
				CheatClasses.AddUnique(Class);
			}
		}
		
		//创建
		for (UClass* CheatClasse : CheatClasses)
		{
			if (CheatClasse && !CheatClasse->HasAllClassFlags(CLASS_Abstract))
			{
				CheatManager->AddCheatManagerExtension(NewObject<UGameplayCheatExtension>(CheatManager, CheatClasse));
			}
		}
	}
}
