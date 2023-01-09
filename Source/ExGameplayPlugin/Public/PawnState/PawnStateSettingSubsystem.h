#pragma once 

#include "CoreMinimal.h"
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
	const UPawnStateAsset* GetGlobalPawnStateAsset(FGameplayTag PawnStateTag);

	UPawnStateAsset* GetStreamingLevelPawnState(UWorld* World, ULevel* Level);
	void RegisterLevelChangeHandler();
	void UnregisterLevelChangeHander();
	void OnLevelAdded(ULevel* Level, UWorld* World);
	void OnLevelRemoved(ULevel* Level, UWorld* World);

private:
	FDelegateHandle LevelAddedHandle;
	FDelegateHandle LevelRemovedHandle;
};