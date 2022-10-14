// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFeatureBPLibrary.h"
#include "GameFeaturesSubsystem.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/GameplayStatics.h"

UGameFeaturesSubsystem* UGameFeatureBPLibrary::GetGameFeatureSubsystem()
{
	return &UGameFeaturesSubsystem::Get();
}

bool UGameFeatureBPLibrary::AddToPluginsList(const FString& PluginFilename)
{
	return IPluginManager::Get().AddToPluginsList(PluginFilename);
}

FString UGameFeatureBPLibrary::GetPluginURLByName(const FString& PluginName)
{
	FString Result;
	if (UGameFeaturesSubsystem::Get().GetPluginURLForBuiltInPluginByName(PluginName, Result))
	{
		return Result;
	}
	else
	{
		Result.Empty();
		return Result;
	}
	return Result;
}

void UGameFeatureBPLibrary::LoadGameFeature(const FString& PluginURL)
{
	if(!PluginURL.IsEmpty())
	{
		UGameFeaturesSubsystem::Get().LoadGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateStatic(&UGameFeatureBPLibrary::OnStatus));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" UGameFeatureBPLibrary::LoadGameFeature error, PluginURL is Empty"));
	}
}

void UGameFeatureBPLibrary::ActiveGameFeature(const FString& PluginURL)
{
	if(!PluginURL.IsEmpty())
	{
		UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL,FGameFeaturePluginDeactivateComplete::CreateStatic(&UGameFeatureBPLibrary::OnStatus));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" UGameFeatureBPLibrary::ActiveGameFeature error, PluginURL is Empty"));
	}
}

void UGameFeatureBPLibrary::LoadBuiltInGameFeaturePlugin(const FString& PluginName)
{
	TSharedPtr<IPlugin> FoundModule = IPluginManager::Get().FindPlugin(PluginName);

	if (FoundModule.IsValid())
	{
		UGameFeaturesSubsystem::FBuiltInPluginAdditionalFilters Filters = [](const FString& PluginFilename, const FGameFeaturePluginDetails& Details, FBuiltInGameFeaturePluginBehaviorOptions& OutOptions)->bool
		{
			UE_LOG(LogTemp,Log,TEXT("Load Game Feature Plugin %s"),*PluginFilename);
			return true;
		};
		UGameFeaturesSubsystem::Get().LoadBuiltInGameFeaturePlugin(FoundModule.ToSharedRef(),Filters);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" UGameFeatureBPLibrary::LoadBuiltInGameFeaturePlugin error, FoundModule for %s is not valid"), *PluginName);
	}
}

void UGameFeatureBPLibrary::LoadBuiltInGameFeaturePlugins()
{
	UGameFeaturesSubsystem::FBuiltInPluginAdditionalFilters Filters = [](const FString& PluginFilename, const FGameFeaturePluginDetails& Details, FBuiltInGameFeaturePluginBehaviorOptions& OutOptions)->bool
	{
		UE_LOG(LogTemp, Log, TEXT("Load Game Feature Plugin %s"),*PluginFilename);
		return true;
	};
	UGameFeaturesSubsystem::Get().LoadBuiltInGameFeaturePlugins(Filters);
}

void UGameFeatureBPLibrary::UnloadGameFeature(const FString& PluginURL, bool bKeepRegistered)
{
	UGameFeaturesSubsystem::Get().UnloadGameFeaturePlugin(PluginURL, bKeepRegistered);
}

void UGameFeatureBPLibrary::DeactivateGameFeature(const FString& PluginURL)
{
	UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
}


void UGameFeatureBPLibrary::OnStatus(const UE::GameFeatures::FResult& InStatus)
{
	if(InStatus.IsValid() && InStatus.HasError())
	{
		UE_LOG(LogTemp,Log,TEXT("Load Status %s"),**InStatus.TryGetError());
	}	
}


void UGameFeatureBPLibrary::SetGameFeatureState(const FString PluginURL, const EBPGameFeatureState State)
{
	if (State == EBPGameFeatureState::Deactivated)
	{
		UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
	}

	if (State == EBPGameFeatureState::Activated)
	{
		UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete());
	}

	if (State == EBPGameFeatureState::Unloaded)
	{
		UGameFeaturesSubsystem::Get().UnloadGameFeaturePlugin(PluginURL);
	}

	if (State == EBPGameFeatureState::Loaded)
	{
		UGameFeaturesSubsystem::Get().LoadGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete());
	}
}