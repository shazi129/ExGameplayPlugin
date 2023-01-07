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

}

void UPawnStateSettingSubsystem::Deinitialize()
{

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

UPawnState* UPawnStateSettingSubsystem::GetStreamingLevelPawnState(UWorld* World, ULevel* Level)
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

	FString WorldPackageFullName = UExGameplayLibrary::GetWorldPackageFullName(World);
	for (const FStreamingLevelState& StateInfo : GetDefault<UPawnStateSettings>()->SteamingLevelState)
	{
		if (WorldPackageFullName != StateInfo.MainWorld.GetLongPackageName())
		{
			continue;
		}

		for (TObjectPtr<ULevelStreaming>& LevelStreaming : World->WorldComposition->TilesStreaming)
		{
			FName SubLevelName = FPackageName::GetShortFName(LevelStreaming->PackageNameToLoad);
			if (StateInfo.LevelState.Contains(SubLevelName))
			{
				return StateInfo.LevelState[SubLevelName];
			}
			return nullptr;
		}
	}

	return nullptr;
}

void UPawnStateSettingSubsystem::OnLevelAdded(ULevel* Level, UWorld* World)
{
	UPawnState* PawnState = GetStreamingLevelPawnState(World, Level);
	if (PawnState)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		UPawnStateLibrary::EnterPawnState(Character, FPawnStateInstance(PawnState, Level, nullptr));
	}
}

void UPawnStateSettingSubsystem::OnLevelRemoved(ULevel* Level, UWorld* World)
{
	UPawnState* PawnState = GetStreamingLevelPawnState(World, Level);
	if (PawnState)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		UPawnStateLibrary::LeavePawnState(Character, FPawnStateInstance(PawnState, Level, nullptr));
	}
}