#include "GameFeaturesWorldManager.h"

FWorldDelegates::FWorldEvent UGameFeaturesWorldManager::OnWorldBeginplayDelegate;

void UGameFeaturesWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("UGameFeaturesWorldManager::Initialize"));
}

void UGameFeaturesWorldManager::PostInitialize()
{
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