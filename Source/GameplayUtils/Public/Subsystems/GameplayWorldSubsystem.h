#pragma once 

/**
 * 游戏中与场景相关，比较通用的一些WorldSubsystem功能
 */

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayWorldSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayWorldEvent, UWorld*, World);

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGameplayWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UGameplayWorldSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

#pragma region //////////////////////////////////World相关的一些回调///////
public:
	UPROPERTY(BlueprintAssignable)
	FGameplayWorldEvent WorldBeginPlayDelegate;

	UPROPERTY(BlueprintAssignable)
	FGameplayWorldEvent WorldTeardownDeletage;

	UPROPERTY(BlueprintAssignable)
	FGameplayWorldEvent WorldInitializedActorsDeletage;

private:
	void OnWorldTearingDown(UWorld* World);
	void OnWorldInitializedActors(const UWorld::FActorsInitializedParams& Params);

private:
	bool bIsWorldBeginplay = false;
	FDelegateHandle WorldTearDownHandler;
	FDelegateHandle WorldInitializedActorsHandler;

#pragma region //////////////////////////////////全局的一些Object缓存////////
public:
	UFUNCTION(BlueprintCallable)
	UObject* GetGlobalObject(FName ObjectName);

	UFUNCTION(BlueprintCallable)
	TArray<UObject*> GetGlobalObjectList(FName ObjectName);

	UFUNCTION(BlueprintCallable)
	bool AddGlobalObject(FName ObjectName, UObject* Object);

	UFUNCTION(BlueprintCallable)
	void RemoveGlobalObjects(FName ObjectName);

private:
	//全局的Object
	UPROPERTY()
	TMap<FName, FObjectListData> GlobalObjectsMap;
#pragma endregion
};