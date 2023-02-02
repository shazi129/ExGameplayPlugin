#pragma once 

#include "CoreMinimal.h"
#include "PawnState/PawnStateSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PawnStateSettingSubsystem.generated.h"

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UPawnStateSettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure)
	static UPawnStateSettingSubsystem* GetSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	const UPawnStateAsset* GetGlobalPawnStateAsset(FGameplayTag PawnStateTag);

	const FWorldPawnStateInfo* GetWorldStateInfo(UWorld* InWorld);

	UPawnStateAsset* GetStreamingLevelPawnState(UWorld* World, ULevel* Level);

	void RegisterLevelChangeHandler();
	void UnregisterLevelChangeHander();

	void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS);
	void OnWorldTearingDown(UWorld* World);

	void OnLevelAdded(ULevel* Level, UWorld* World);
	void OnLevelRemoved(ULevel* Level, UWorld* World);

private:
	FDelegateHandle LevelAddedHandle;
	FDelegateHandle LevelRemovedHandle;

	FDelegateHandle PostWorldInitHandle;
	FDelegateHandle WorldBeginTearDownHandler;
};