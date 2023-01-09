#include "PawnState/PawnStateSettingSubsystem.h"
#include "PawnState/PawnStateSettings.h"
#include "PawnState/PawnStateLibrary.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ExGameplayLibrary.h"

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
	LevelAddedHandle = FWorldDelegates::LevelAddedToWorld.AddUObject(this, &UPawnStateSettingSubsystem::OnLevelAdded);
	LevelRemovedHandle = FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &UPawnStateSettingSubsystem::OnLevelRemoved);
}
void UPawnStateSettingSubsystem::UnregisterLevelChangeHander()
{
	FWorldDelegates::LevelAddedToWorld.Remove(LevelAddedHandle);
	FWorldDelegates::LevelRemovedFromWorld.Remove(LevelRemovedHandle);
}

UPawnStateAsset* UPawnStateSettingSubsystem::GetStreamingLevelPawnState(UWorld* World, ULevel* Level)
{
	//只在客户端生效
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
	for (const FStreamingLevelState& StateInfo : GetDefault<UPawnStateSettings>()->SteamingLevelState)
	{
		if (WorldPackageFullName != StateInfo.MainWorld.GetLongPackageName())
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

void UPawnStateSettingSubsystem::OnLevelAdded(ULevel* Level, UWorld* World)
{
	UPawnStateAsset* PawnState = GetStreamingLevelPawnState(World, Level);
	if (PawnState)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		UPawnStateLibrary::EnterPawnState(Character, FPawnStateInstance(PawnState, Level, nullptr));
	}
}

void UPawnStateSettingSubsystem::OnLevelRemoved(ULevel* Level, UWorld* World)
{
	UPawnStateAsset* PawnState = GetStreamingLevelPawnState(World, Level);
	if (PawnState)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		UPawnStateLibrary::LeavePawnState(Character, FPawnStateInstance(PawnState, Level, nullptr));
	}
}

const UPawnStateAsset* UPawnStateSettingSubsystem::GetGlobalPawnStateAsset(FGameplayTag PawnStateTag)
{
	for (const UPawnStateAsset* Asset : GetDefault<UPawnStateSettings>()->GlobalPawnStateAssets.Assets)
	{
		if (Asset->PawnState.PawnStateTag == PawnStateTag)
		{
			return Asset;
		}
	}
	return nullptr;
}