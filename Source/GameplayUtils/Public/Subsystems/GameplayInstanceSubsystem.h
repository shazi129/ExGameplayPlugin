#pragma once

/**
 * 游戏中一些通用的GameInstanceSystem功能, 例如保存一些值, World生命周期的监听等
 */

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayInstanceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayWorldEvent, UWorld*, World);

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGameplayInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UGameplayInstanceSubsystem* Get(const UObject* WorldContextObject);

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#pragma region //////////////////////////////////World相关的一些回调///////
public:
	UPROPERTY(BlueprintAssignable)
	FGameplayWorldEvent WorldInitializedActorsDeletage;

	UPROPERTY(BlueprintAssignable)
	FGameplayWorldEvent WorldBeginPlayDelegate;

	UPROPERTY(BlueprintAssignable)
	FGameplayWorldEvent WorldTeardownDeletage;

private:
	void OnWorldTearingDown(UWorld* World);
	void OnWorldInitializedActors(const UWorld::FActorsInitializedParams& Params);

private:
	bool bIsWorldBeginplay = false;
	FDelegateHandle WorldTearDownHandler;
	FDelegateHandle WorldInitializedActorsHandler;
#pragma endregion ---------------------------World相关的一些回调------------
};
