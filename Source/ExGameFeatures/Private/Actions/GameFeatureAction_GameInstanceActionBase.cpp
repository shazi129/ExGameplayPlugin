#include "Actions/GameFeatureAction_GameInstanceActionBase.h"
#include "ExGameFeaturesModule.h"

void UGameFeatureAction_GameInstanceActionBase::OnGameFeatureActivating()
{
	EXIGAMEFEATURE_LOG(Log, TEXT("%s %s"), *FString(__FUNCTION__), *(GetName()));
	
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		if (World && World->IsGameWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (HandledGameInstances.Find(GameInstance) == INDEX_NONE && AddToGameInstance(GameInstance))
				{
					HandledGameInstances.Add(GameInstance);
				}
			}
		}
	}

	GameInstanceStartHandle = FWorldDelegates::OnStartGameInstance.AddUObject(this, &UGameFeatureAction_GameInstanceActionBase::HandleGameInstanceStart);
}

void UGameFeatureAction_GameInstanceActionBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	EXIGAMEFEATURE_LOG(Log, TEXT("%s %s"), *FString(__FUNCTION__), *(GetName()));
	FWorldDelegates::OnStartGameInstance.Remove(GameInstanceStartHandle);
	HandledGameInstances.Reset();
}


bool UGameFeatureAction_GameInstanceActionBase::AddToGameInstance(UGameInstance* GameInstance)
{
	return false;
}

void UGameFeatureAction_GameInstanceActionBase::HandleGameInstanceStart(UGameInstance* GameInstance)
{
	if (HandledGameInstances.Find(GameInstance) == INDEX_NONE && AddToGameInstance(GameInstance))
	{
		EXIGAMEFEATURE_LOG(Log, TEXT("%s %s"), *FString(__FUNCTION__), *(GetName()));
		HandledGameInstances.Add(GameInstance);
	}
}