// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/GameFeatureAction_AddActorComponents.h"
#include "AssetRegistry/AssetBundleData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/GameInstance.h"
#include "GameFeaturesSubsystemSettings.h"
#include "Engine/AssetManager.h"
#include "ExGameFeaturesModule.h"

#define LOCTEXT_NAMESPACE "ExGameFeatures"

void UGameFeatureAction_AddActorComponents::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	ComponentRequestHandles.Empty();
}

#if WITH_EDITORONLY_DATA
void UGameFeatureAction_AddActorComponents::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	if (UAssetManager::IsValid())
	{
		for (const FAddActorComponentEntry& Entry : ComponentList)
		{
			if (Entry.bClientComponent)
			{
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, Entry.ComponentClass.ToSoftObjectPath().GetAssetPath());
			}
			if (Entry.bServerComponent)
			{
				AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateServer, Entry.ComponentClass.ToSoftObjectPath().GetAssetPath());
			}
		}
	}
}
#endif

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddActorComponents::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FAddActorComponentEntry& Entry : ComponentList)
	{
		if (Entry.ActorClass.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			ValidationErrors.Add(FText::Format(LOCTEXT("ComponentEntryHasNullActor", "Null ActorClass at index {0} in ComponentList"), FText::AsNumber(EntryIndex)));
		}

		if (Entry.ComponentClass.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			ValidationErrors.Add(FText::Format(LOCTEXT("ComponentEntryHasNullComponent", "Null ComponentClass at index {0} in ComponentList"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddActorComponents::AddToGameInstance(UGameInstance* GameInstance)
{
	if (!GameInstance)
	{
		return;
	}

	UWorld* World = GameInstance->GetWorld();
	if (World != nullptr && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* GFCM = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			const ENetMode NetMode = World->GetNetMode();
			const bool bIsServer = NetMode != NM_Client;
			const bool bIsClient = NetMode != NM_DedicatedServer;

			for (const FAddActorComponentEntry& Entry : ComponentList)
			{
				EXIGAMEFEATURE_LOG(Log, TEXT("%s: %s --> %s (client: %d, server: %d)"), *FString(__FUNCTION__),
					*Entry.ComponentClass.ToString(), *Entry.ActorClass.ToString(), bIsClient ? 1 : 0, bIsServer ? 1 : 0);

				const bool bShouldAddRequest = (bIsServer && Entry.bServerComponent) || (bIsClient && Entry.bClientComponent);
				if (bShouldAddRequest)
				{
					if (!Entry.ActorClass.IsNull() && !Entry.ComponentClass.IsNull())
					{
						UE_SCOPED_ENGINE_ACTIVITY(TEXT("Adding component to world %s (%s)"), *World->GetDebugDisplayName(), *Entry.ComponentClass.ToString());
						TSubclassOf<UActorComponent> ComponentClass = Entry.ComponentClass.LoadSynchronous();
						if (ComponentClass)
						{
							ComponentRequestHandles.Add(GFCM->AddComponentRequest(Entry.ActorClass, ComponentClass));
						}
						else
						{
							EXIGAMEFEATURE_LOG(Log, TEXT("%s: cannot load component class:%s"), *FString(__FUNCTION__), *Entry.ComponentClass.ToString());
						}
					}
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

