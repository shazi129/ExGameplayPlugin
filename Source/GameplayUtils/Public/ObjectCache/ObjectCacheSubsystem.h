#pragma once

/**
 * UE对象池，主要针对UObject及其各种基类
 * 
 * *****基本使用方式*****
 * 
 * 创建对象池：
 * UObjectCacheSubsystem* CacheSubsystem = UObjectCacheSubsystem::GetSubsystem(this);
 * CacheSubsystem->CreateObjectPool(FActorCachePool::StaticStruct(), BombClass, GetWorld(), 3);  //创建一个BombClass的Actor池
 * 
 * 获取一个对象
 * BombActor = Cast<AIsomersBombActor>(CacheSubsystem->RetainObject(BombClass));
 * 
 * 回收一个对象
 * CacheSubsystem->ReleaseObject(BombActor)
 * 
 * 销毁对象池：
 * CacheSubsystem->DestroyObjectPool(BombClass);
 * 
 * *******一些特殊的对象**********
 * 游戏中有些对象可能需要在 创建/获取/回收/销毁 时做一些回调操作，这时候就需要自定义对象池类型：
 * 
 * USTRUCT(BlueprintType)
 * struct ISOMERSFRAMEWORK_API FIsomersBombCache : public FActorCachePool
 * {
 * 		GENERATED_BODY()
 *	public:
 * 		FIsomersBombCache() {}
 * 		virtual void OnObjectCreate(UObject* Item) override;
 * 		virtual void OnObjectDestroy(UObject* Item) override;
 * 		virtual void OnObjectRetain(UObject* Item) override;
 * 		virtual void OnObjectRelease(UObject* Item) override;
 * };
 * 
 * 创建自定义对象池：
 * CacheSubsystem->CreateObjectPool(FIsomersBombCache::StaticStruct(), BombClass, GetWorld(), 3)
 *
 */

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "ObjectCache/ObjectCacheReplicateActor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectCache/ObjectCachePool.h"
#include "ObjectCacheSubsystem.generated.h"



DECLARE_DYNAMIC_DELEGATE_OneParam(FOnBeforeRetainDelegate, AActor*, Actor);

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UObjectCacheSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UObjectCacheSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize();

	UFUNCTION(BlueprintCallable)
		bool CreateObjectPool(const TSubclassOf<UObjectCachePool>& InPoolClass, UClass* InObjectClass, UObject* SourceObject = nullptr, int InitSize=0);

	UFUNCTION(BlueprintCallable)
		bool DestroyObjectPool(const UClass* InObjectClass);

	UFUNCTION(BlueprintCallable)
		UObject* RetainObject(const UClass* ObjectClass);
	
	UFUNCTION(BlueprintCallable)
		AActor* RetainActor(UClass* ObjectClass, const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable)
		AActor* RetainActorWithDelegate(UClass* ObjectClass, const FTransform& SpawnTransform, const FOnBeforeRetainDelegate& Delegate);

	UFUNCTION(BlueprintCallable)
		bool ReleaseObject(UObject* Object, const UClass* ObjectClass=nullptr);

	UFUNCTION(BlueprintCallable)
		bool CanWorldUseObjectPool(const UClass* ObjectClass);

private:
	void OnWorldInitializedActors(const UWorld::FActorsInitializedParams& Params);
	void OnWorldTearingDown(UWorld* World);

private:
	FDelegateHandle WorldBeginTearDownHandler;
	FDelegateHandle WorldInitializedActorsHandler;

	UPROPERTY()
	TArray<UObjectCachePool*> ClassObjectCachePool;

	UPROPERTY()
	TMap<TWeakObjectPtr<UWorld>, TWeakObjectPtr<AObjectCacheReplicateActor>> ReplicateActorMap;
};
