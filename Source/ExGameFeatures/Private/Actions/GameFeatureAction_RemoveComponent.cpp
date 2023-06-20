#include "Actions/GameFeatureAction_RemoveComponent.h"
#include "Engine/AssetManager.h"
#include "GameFeaturesSubsystemSettings.h"
#include "ExGameFeaturesModule.h"

#define LOCTEXT_NAMESPACE "GameFeatures"


void UGameFeatureAction_RemoveComponent::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	ComponentRequestHandles.Empty();
}

#if WITH_EDITORONLY_DATA
void UGameFeatureAction_RemoveComponent::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	if (UAssetManager::IsValid())
	{
		for (const FRedundantComponentEntry& Entry : ComponentList)
		{
			AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, Entry.OwnerActorClass.ToSoftObjectPath().GetAssetPath());
			AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateServer, Entry.OwnerActorClass.ToSoftObjectPath().GetAssetPath());
		}
	}
}
#endif

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_RemoveComponent::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FRedundantComponentEntry& Entry : ComponentList)
	{
		if (Entry.OwnerActorClass.IsNull())
		{
			ValidationErrors.Add(FText::FromString("Null OwnerActor Class for UGameFeatureAction_RemoveComponent."));
		}
	}

	return Result;
}
#endif

void UGameFeatureAction_RemoveComponent::AddToWorld(const FWorldContext& WorldContext)
{
	EXIGAMEFEATURE_LOG(Log, TEXT("%s start"), *FString(__FUNCTION__));

	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	if ((World != nullptr) && World->IsGameWorld() && GameInstance)
	{
		if (UGameFrameworkComponentManager* GFCM = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			ENetMode NetModel = World->GetNetMode();
			for (const FRedundantComponentEntry& Entry : ComponentList)
			{
				if ((NetModel == ENetMode::NM_DedicatedServer && !Entry.bInDedicateServer)
					|| (NetModel != ENetMode::NM_DedicatedServer && !Entry.bInClient))
				{
					continue;
				}

				//GFCM->GetRe

				UGameFrameworkComponentManager::FExtensionHandlerDelegate HandlerDelegate;
				HandlerDelegate.BindUObject(this, &UGameFeatureAction_RemoveComponent::OnOwnerActorAdded);
				ComponentRequestHandles.Add(GFCM->AddExtensionHandler(Entry.OwnerActorClass, HandlerDelegate));
			}
		}
	}
}

void UGameFeatureAction_RemoveComponent::OnOwnerActorAdded(AActor* OwnerActor, FName AddTag)
{
	EXIGAMEFEATURE_LOG(Log, TEXT("%s start, OwnerActor[%s], AddTag[%s]"), *FString(__FUNCTION__), *GetNameSafe(OwnerActor), *AddTag.ToString());

	if (OwnerActor != nullptr)
	{
		RemoveComponents(OwnerActor);
	}
	
}

void UGameFeatureAction_RemoveComponent::RemoveComponents(AActor* OwnerActor)
{
	if (OwnerActor)
	{
		UClass* OwnerActorClassPtr = OwnerActor->GetClass();
		for (const FRedundantComponentEntry& Entry : ComponentList)
		{
			if (OwnerActorClassPtr->IsChildOf(Entry.OwnerActorClass.Get()))
			{
				for (const FName& ComponentTag : Entry.ComponentTags)
				{
					TArray<UActorComponent*> ActorCompoents = OwnerActor->GetComponentsByTag(UActorComponent::StaticClass(), ComponentTag);
					for (UActorComponent* Component : ActorCompoents)
					{
						Component->DestroyComponent();
						Component->SetFlags(RF_Transient);
					}
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE