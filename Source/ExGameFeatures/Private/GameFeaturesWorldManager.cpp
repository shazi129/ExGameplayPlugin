#include "GameFeaturesWorldManager.h"

FWorldDelegates::FWorldEvent UGameFeaturesWorldManager::OnWorldBeginplayDelegate;

void UGameFeaturesWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("UGameFeaturesWorldManager::Initialize"));
}

void UGameFeaturesWorldManager::PostInitialize()
{
	for (auto& PreExistingInstance : SystemInstances)
	{
		PreExistingInstance.Key->Initialize(GetWorld());
	}

	bIsInitialized = true;
}

bool UGameFeaturesWorldManager::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UGameFeaturesWorldManager::OnWorldBeginPlay(UWorld& InWorld)
{
	bIsWorldBeginplay = true;

	if (OnWorldBeginplayDelegate.IsBound())
	{
		UWorld* World = GetWorld();
		OnWorldBeginplayDelegate.Broadcast(World);
	}
}

bool UGameFeaturesWorldManager::IsBeginPlay()
{
	return bIsWorldBeginplay;
}

UGameFeatureWorldSystem* UGameFeaturesWorldManager::RequestSystemOfType(TSubclassOf<UGameFeatureWorldSystem> SystemType)
{
	for (auto& PreExistingInstance : SystemInstances)
	{
		if (PreExistingInstance.Key->GetClass() == SystemType)
		{
			PreExistingInstance.Value += 1;
			return PreExistingInstance.Key;
		}
	}

	UGameFeatureWorldSystem* NewWorldSystem = NewObject<UGameFeatureWorldSystem>(GetWorld(), SystemType);
	SystemInstances.Add(NewWorldSystem, 1);

	if (bIsInitialized)
	{
		NewWorldSystem->Initialize(GetWorld());
	}

	return NewWorldSystem;
}

void UGameFeaturesWorldManager::ReleaseRequestForSystemOfType(TSubclassOf<UGameFeatureWorldSystem> SystemType)
{
	for (auto& PreExistingInstance : SystemInstances)
	{
		if (PreExistingInstance.Key->GetClass() == SystemType)
		{
			PreExistingInstance.Value -= 1;

			if (PreExistingInstance.Value <= 0)
			{
				SystemInstances.Remove(PreExistingInstance.Key);
			}
			break;
		}
	}
}

UGameFeatureWorldSystem* UGameFeaturesWorldManager::FindGameFeatureWorldSystemOfType(const UObject* WorldContextObject, TSubclassOf<UGameFeatureWorldSystem> SystemType)
{
	UGameFeatureWorldSystem* WorldSystemInst = nullptr;
	if (WorldContextObject)
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			if (UGameFeaturesWorldManager* SystemManager = World->GetSubsystem<UGameFeaturesWorldManager>())
			{
				for (auto& WorldSystem : SystemManager->SystemInstances)
				{
					if (WorldSystem.Key && WorldSystem.Key->GetClass() == SystemType)
					{
						WorldSystemInst = WorldSystem.Key;
						break;
					}
				}
			}
		}
	}
	return WorldSystemInst;
}