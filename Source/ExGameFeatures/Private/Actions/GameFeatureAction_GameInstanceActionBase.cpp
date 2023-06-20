#include "Actions/GameFeatureAction_GameInstanceActionBase.h"
#include "ExGameFeaturesModule.h"

void UGameFeatureAction_GameInstanceActionBase::OnGameFeatureActivating()
{
	EXIGAMEFEATURE_LOG(Log, TEXT("%s %s"), *FString(__FUNCTION__), *(GetName()));
	
	TMap<UGameInstance*, int> HandledGameInstance;

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		if (World && World->IsGameWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (!HandledGameInstance.Contains(GameInstance))
				{
					AddToGameInstance(GameInstance);
					HandledGameInstance.Add(GameInstance, 1);
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
}

void UGameFeatureAction_GameInstanceActionBase::AddToGameInstance(UGameInstance* GameInstance)
{
	
}

void UGameFeatureAction_GameInstanceActionBase::HandleGameInstanceStart(UGameInstance* GameInstance)
{
	EXIGAMEFEATURE_LOG(Log, TEXT("%s %s"), *FString(__FUNCTION__), *(GetName()));
	AddToGameInstance(GameInstance);
}