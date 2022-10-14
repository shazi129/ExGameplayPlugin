#include "Settings/GameplaySettingSubsystem.h"
#include "Settings/GameplaySettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFeaturesSubsystem.h"

UGameplaySettingSubsystem* UGameplaySettingSubsystem::GetGameplaySettingSubsystem(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UGameplaySettingSubsystem>();
}

FString UGameplaySettingSubsystem::GetString(FString ConfigKey)
{
	FString ConfigValue = "";
	bool Result = GConfig->GetString(TEXT("/Script/soc_client.GameplaySettings"), *ConfigKey, ConfigValue, ConfigFilePath);
	if (!Result)
	{
		UE_LOG(LogTemp, Error, TEXT("UGameplaySettingSubsystem.GetString error, Key[%s], return default[%s]"), *ConfigKey, *ConfigValue);
	}
	return ConfigValue;
}

bool UGameplaySettingSubsystem::GetBool(FString ConfigKey)
{
	bool ConfigValue = false;
	bool Result = GConfig->GetBool(TEXT("/Script/soc_client.GameplaySettings"), *ConfigKey, ConfigValue, ConfigFilePath);
	if (!Result)
	{
		UE_LOG(LogTemp, Error, TEXT("UGameplaySettingSubsystem.GetBool error, Key[%s], return defalut[%d]"), *ConfigKey, ConfigValue);
	}
	return ConfigValue;
}

int UGameplaySettingSubsystem::GetInt(FString ConfigKey)
{
	int ConfigValue = 0;
	bool Result = GConfig->GetInt(TEXT("/Script/soc_client.GameplaySettings"), *ConfigKey, ConfigValue, ConfigFilePath);
	if (!Result)
	{
		UE_LOG(LogTemp, Error, TEXT("UGameplaySettingSubsystem.GetInt error, Key[%s], return default[%d]"), *ConfigKey, ConfigValue);
	}
	return ConfigValue;
}

void UGameplaySettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ActivatedGameFeaturesNames.Empty();
	ConfigFilePath = FPaths::Combine(FPaths::SourceConfigDir(), TEXT("DefaultGameplay.ini"));

	HandleSettings();
	LoadAndAcitvateDefaultFeatures();

#if UE_WITH_CHEAT_MANAGER
	CheatManagerCreateHandle = UCheatManager::RegisterForOnCheatManagerCreated(
			FOnCheatManagerCreated::FDelegate::CreateUObject(this, &UGameplaySettingSubsystem::OnCheatManagerCreate));
#endif
}

void UGameplaySettingSubsystem::Deinitialize()
{
	Super::Deinitialize();

	//GameFeatures
	UGameFeaturesSubsystem& GFS = UGameFeaturesSubsystem::Get();
	FString PluginURL;
	for (int i = 0; i < ActivatedGameFeaturesNames.Num(); i++)
	{
		PluginURL.Empty();
		bool Result = GFS.GetPluginURLForBuiltInPluginByName(ActivatedGameFeaturesNames[i], PluginURL);
		if (Result && !PluginURL.IsEmpty())
		{
			GFS.UnloadGameFeaturePlugin(PluginURL);
			GFS.DeactivateGameFeaturePlugin(PluginURL);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGameplaySettingSubsystem::Deinitialize error, cannot get plugin url of %s"), *(ActivatedGameFeaturesNames[i]));
			continue;
		}
	}
	ActivatedGameFeaturesNames.Empty();

	if (CheatManagerCreateHandle.IsValid())
	{
		UCheatManager::UnregisterFromOnCheatManagerCreated(CheatManagerCreateHandle);
	}
}

void UGameplaySettingSubsystem::HandleSettings()
{
	HandleDefaultGameFeatures();
}


void UGameplaySettingSubsystem::HandleDefaultGameFeatures()
{
	const TArray<FString>& DefaultFeatures = GetDefault<UGameplaySettings>()->DefaultGameFeatures;
	for (int i = 0; i < DefaultFeatures.Num(); i++)
	{
		ActivatedGameFeaturesNames.AddUnique(DefaultFeatures[i]);
	}
}

void UGameplaySettingSubsystem::LoadAndAcitvateDefaultFeatures()
{
	UGameFeaturesSubsystem& GFS = UGameFeaturesSubsystem::Get();
	FString PluginURL;

	for (int i = 0; i < ActivatedGameFeaturesNames.Num(); i++)
	{
		PluginURL.Empty();
		bool Result = UGameFeaturesSubsystem::Get().GetPluginURLForBuiltInPluginByName(ActivatedGameFeaturesNames[i], PluginURL);
		if (Result && !PluginURL.IsEmpty())
		{
			GFS.LoadGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete());
			GFS.LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginDeactivateComplete());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGameplaySettingSubsystem::LoadAndAcitvateDefaultFeatures error, cannot get plugin url of %s"), *(ActivatedGameFeaturesNames[i]));
			continue;
		}
	}
}

void UGameplaySettingSubsystem::OnCheatManagerCreate(UCheatManager* CheatManager)
{
	if (CheatManager)
	{
		auto CheatExtensionClasses = GetDefault<UGameplaySettings>()->CheatExtensionClasses;
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