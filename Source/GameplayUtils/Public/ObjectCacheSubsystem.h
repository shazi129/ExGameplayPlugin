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
#include "Macros/SubsystemMacros.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectCacheSubsystem.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FClassObjectCachePool
{
	friend class UObjectCacheSubsystem;

	GENERATED_BODY()

public:
	struct FCacheObjectItem
	{
		UPROPERTY(transient)
			TSoftObjectPtr<UObject> Object;

		int Status;

		void Reset();

		bool IsValid();
	};

public:
	FClassObjectCachePool() {};
	virtual ~FClassObjectCachePool() {};

	UWorld* GetWorld() { return ContextWorld; }
	UClass* GetClass() { return ObjectClass; }

	void SetContextWorld(UWorld* InContextWorld) { ContextWorld = InContextWorld; }
	void SetObjectClass(UClass* InObjectClass) { ObjectClass = InObjectClass; }
	void SetDesigneSize(int Size) { DesignSize = Size; }

	virtual void InitializePool();
	virtual void DeinitializePool() {};

	virtual UObject* CreateObject() { return nullptr; }

	virtual UObject* Retain();

	virtual bool Release(UObject* Item);

	virtual void OnObjectCreate(UObject* Item) {}
	virtual void OnObjectDestroy(UObject* Item) {}
	virtual void OnObjectRetain(UObject* Item) {}
	virtual void OnObjectRelease(UObject* Item) {}

protected:
	//类型
	UClass* ObjectClass;

	//所有Object都是基于
	UWorld* ContextWorld;

	TArray<FCacheObjectItem> CacheObjects;
	int DesignSize;

	int ReferenceCount = 0;
};

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FActorCachePool : public FClassObjectCachePool
{
	GENERATED_BODY()
public:
	FActorCachePool() {};
	virtual void DeinitializePool() override;
	virtual UObject* CreateObject();
};

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UObjectCacheSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_GET_GAMEINSTANCE_SUBSYSTEM(UObjectCacheSubsystem, LogTemp)

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize();
	void OnWorldTearingDown(UWorld* World);

	UFUNCTION(BlueprintCallable)
		bool CreateObjectPool(UScriptStruct* InScriptStruct, UClass* ObjectClass, UWorld* ContextWorld, int DesignSize);

	UFUNCTION(BlueprintCallable)
		bool DestroyObjectPool(UClass* Class, bool OnlyClearItems = true);

	UFUNCTION(BlueprintCallable)
		UObject* RetainObject(const UClass* ObjectClass);

	UFUNCTION(BlueprintCallable)
		bool ReleaseObject(UObject* Object, const UClass* Class=nullptr);

private:
	FDelegateHandle WorldBeginTearDownHandler;

	TArray<FClassObjectCachePool*> ClassObjectCachePool;
};
