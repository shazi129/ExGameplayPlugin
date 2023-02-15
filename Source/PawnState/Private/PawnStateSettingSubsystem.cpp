#include "PawnStateSettingSubsystem.h"
#include "PawnStateLibrary.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ExGameplayLibrary.h"
#include "PawnStateModule.h"
#include "Engine/World.h"

UPawnStateSettingSubsystem* UPawnStateSettingSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UPawnStateSettingSubsystem>();
}

void UPawnStateSettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		if (UExGameplayLibrary::IsClient(World))
		{
			RegisterLevelChangeHandler();
		}
	}
}

void UPawnStateSettingSubsystem::Deinitialize()
{
	Super::Deinitialize();
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		if (UExGameplayLibrary::IsClient(World))
		{
			UnregisterLevelChangeHander();
		}
	}
}

void UPawnStateSettingSubsystem::RegisterLevelChangeHandler()
{
	PAWNSTATE_LOG(Log, TEXT("%s"), *FString(__FUNCTION__));

	PostWorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UPawnStateSettingSubsystem::OnPostWorldInit);
	WorldBeginTearDownHandler = FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UPawnStateSettingSubsystem::OnWorldTearingDown);
	LevelAddedHandle = FWorldDelegates::LevelAddedToWorld.AddUObject(this, &UPawnStateSettingSubsystem::OnLevelAdded);
	LevelRemovedHandle = FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &UPawnStateSettingSubsystem::OnLevelRemoved);

	const TIndirectArray<FWorldContext>& WorldContextList = GEngine->GetWorldContexts();
	for (const FWorldContext& WorldContext : WorldContextList)
	{
		UWorld* World = WorldContext.World();
		if (World && World->IsGameWorld())
		{
			PAWNSTATE_LOG(Log, TEXT("%s handle already exist world[%s]"), *FString(__FUNCTION__), *World->GetPackage()->GetName());
			OnPostWorldInit(World, UWorld::InitializationValues());

			if (World->WorldComposition)
			{
				for (ULevel* Level : World->GetLevels())
				{
					OnLevelAdded(Level, World);
				}
			}
		}
	}
}

void UPawnStateSettingSubsystem::UnregisterLevelChangeHander()
{
	PAWNSTATE_LOG(Log, TEXT("%s"), *FString(__FUNCTION__));

	FWorldDelegates::OnPostWorldInitialization.Remove(PostWorldInitHandle);
	FWorldDelegates::OnWorldBeginTearDown.Remove(WorldBeginTearDownHandler);
	FWorldDelegates::LevelAddedToWorld.Remove(LevelAddedHandle);
	FWorldDelegates::LevelRemovedFromWorld.Remove(LevelRemovedHandle);
}

const FWorldPawnStateInfo* UPawnStateSettingSubsystem::GetWorldStateInfo(UWorld* InWorld)
{
	FString WorldPackageFullName = UExGameplayLibrary::GetPackageFullName(InWorld, InWorld);
	for (const FWorldPawnStateInfo& StateInfo : GetDefault<UPawnStateSettings>()->WorldPawnStates)
	{
		if (!StateInfo.MainWorld.IsNull() && WorldPackageFullName == StateInfo.MainWorld.GetLongPackageName())
		{
			return &StateInfo;
		}
	}
	return nullptr;
}

UPawnStateAsset* UPawnStateSettingSubsystem::GetStreamingLevelPawnState(UWorld* World, ULevel* Level)
{
	if (UExGameplayLibrary::IsClient(World) == false)
	{
		return nullptr;
	}
	if (!World || !World->WorldComposition)
	{
		return nullptr;
	}

	ACharacter* LocalCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);

	FString WorldPackageFullName = UExGameplayLibrary::GetPackageFullName(World, World);
	for (const FWorldPawnStateInfo& StateInfo : GetDefault<UPawnStateSettings>()->WorldPawnStates)
	{
		if (!StateInfo.MainWorld.IsNull() && WorldPackageFullName != StateInfo.MainWorld.GetLongPackageName())
		{
			continue;
		}

		FString LevelPackageFullName = UExGameplayLibrary::GetPackageFullName(World, Level);
		FName LevelPackageShortName = FPackageName::GetShortFName(LevelPackageFullName);
		if (StateInfo.LevelState.Contains(LevelPackageShortName))
		{
			return StateInfo.LevelState[LevelPackageShortName];
		}
	}

	return nullptr;
}

void UPawnStateSettingSubsystem::OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (!World)
	{
		return;
	}
	FString WorldName = *FPackageName::GetShortName(World->GetPackage()->GetName());
	PAWNSTATE_LOG(Log, TEXT("%s handle post world init [%s]"), *FString(__FUNCTION__), *WorldName);

}

void UPawnStateSettingSubsystem::OnWorldTearingDown(UWorld* World)
{
	if (!World)
	{
		return;
	}

	FString WorldName = *FPackageName::GetShortName(World->GetPackage()->GetName());
	PAWNSTATE_LOG(Log, TEXT("%s handle tearing down world[%s]"), *FString(__FUNCTION__), *WorldName);
	
	if (World->WorldComposition)
	{
		for (ULevel* Level : World->GetLevels())
		{
			OnLevelRemoved(Level, World);
		}
	}

	const FWorldPawnStateInfo* StateInfo = GetWorldStateInfo(World);
	if (StateInfo && StateInfo->WorldState && StateInfo->WorldState->PawnState.IsValid())
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		UPawnStateLibrary::LeavePawnState(Character, FPawnStateInstance(StateInfo->WorldState, World, nullptr));
	}
}

void UPawnStateSettingSubsystem::OnLevelAdded(ULevel* Level, UWorld* World)
{
	if (!World || !Level)
	{
		PAWNSTATE_LOG(Log, TEXT("%s World Or Level is null"), *FString(__FUNCTION__));
		return;
	}

	FString WorldName = *FPackageName::GetShortName(World->GetPackage()->GetName());
	FString LevelName = *FPackageName::GetShortName(Level->GetPackage()->GetName());

	UPawnStateAsset* PawnState = GetStreamingLevelPawnState(World, Level);
	if (PawnState)
	{
		PAWNSTATE_LOG(Log, TEXT("%s World[%s] Level[%s] PawnState[%s]"), *FString(__FUNCTION__), *WorldName, *LevelName, *PawnState->ToString());
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		UPawnStateLibrary::EnterPawnState(Character, FPawnStateInstance(PawnState, Level, nullptr));
	}
	else
	{
		PAWNSTATE_LOG(Log, TEXT("%s World[%s] Level[%s] PawnState[null]"), *FString(__FUNCTION__), *WorldName, *LevelName);
	}
}

void UPawnStateSettingSubsystem::OnLevelRemoved(ULevel* Level, UWorld* World)
{
	if (!World || !Level)
	{
		PAWNSTATE_LOG(Log, TEXT("%s World Or Level is null"), *FString(__FUNCTION__));
		return;
	}

	FString WorldName = *FPackageName::GetShortName(World->GetPackage()->GetName());
	FString LevelName = *FPackageName::GetShortName(Level->GetPackage()->GetName());

	UPawnStateAsset* PawnState = GetStreamingLevelPawnState(World, Level);
	if (PawnState)
	{
		PAWNSTATE_LOG(Log, TEXT("%s World[%s] Level[%s] PawnState[%s]"), *FString(__FUNCTION__), *WorldName, *LevelName, *PawnState->ToString());
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		UPawnStateLibrary::LeavePawnState(Character, FPawnStateInstance(PawnState, Level, nullptr));
	}
	else
	{
		PAWNSTATE_LOG(Log, TEXT("%s World[%s] Level[%s] PawnState[null]"), *FString(__FUNCTION__), *WorldName, *LevelName);
	}
}

const UPawnStateAsset* UPawnStateSettingSubsystem::GetGlobalPawnStateAsset(FGameplayTag PawnStateTag)
{
	if (PawnStateTag.IsValid() == false)
	{
		PAWNSTATE_LOG(Error, TEXT("%s error, Invalid PawnState Tag"), *FString(__FUNCTION__));
		return nullptr;
	}

	for (const UPawnStateAsset* Asset : GetDefault<UPawnStateSettings>()->GlobalPawnStateAssets.Assets)
	{
		if (Asset->PawnState.PawnStateTag == PawnStateTag)
		{
			return Asset;
		}
	}
	return nullptr;
}

