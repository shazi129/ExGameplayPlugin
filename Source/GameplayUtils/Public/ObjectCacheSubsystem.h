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
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectCacheSubsystem.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FClassObjectCachePool
{
	friend class UObjectCacheSubsystem;
	GENERATED_BODY()

	FClassObjectCachePool() 
	{
		Reference = 0;
	}
	virtual ~FClassObjectCachePool() {};
	
protected:
	//创建和销毁对象池
	virtual void InitializePool();
	virtual void DeinitializePool();

	//创建或销毁对象的逻辑
	virtual UObject* CreateObject() { return nullptr; }
	virtual void DestroyObject(UObject* Object){}

	//从对象池中拿或者放
	virtual UObject* Retain();
	virtual bool Release(UObject* Item);

	//各事件的回调
	virtual void OnObjectCreate(UObject* Item) {}
	virtual void OnObjectDestroy(UObject* Item) {}
	virtual void OnObjectRetain(UObject* Item) {}
	virtual void OnObjectRelease(UObject* Item) {}

private:
	void SetCacheInfo(UObject* InSourceObject, UClass* InObjectClass)
	{
		SourceObject = InSourceObject;
		ContextWorld = InSourceObject ? InSourceObject->GetWorld() : nullptr;
		ObjectClass = InObjectClass;
	}

public:
	//类型
	UPROPERTY(EditAnywhere)
	UClass* ObjectClass;

	//初始大小
	UPROPERTY(EditAnywhere)
	int InitSize;

protected:
	//这个缓冲池由谁创建
	TWeakObjectPtr<UObject> SourceObject;

	//所有Object都是基于一个World
	UWorld* ContextWorld;

	//缓存的数据
	struct FCacheObjectItem
	{
		TWeakObjectPtr<UObject> Object;
		int Status; //0:没在用，1:正在使用
		void Reset();
	};
	TArray<FCacheObjectItem> CacheObjects;

	//引用计数
	int Reference = 0;
};

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FActorCachePool : public FClassObjectCachePool
{
	GENERATED_BODY()
public:
	FActorCachePool() {};
	virtual UObject* CreateObject() override;
	virtual void DestroyObject(UObject* Object) override;
};

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UObjectCacheSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UObjectCacheSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize();
	void OnWorldTearingDown(UWorld* World);

	UFUNCTION(BlueprintCallable)
		bool CreateObjectPool(const UScriptStruct* InScriptStruct, UClass* InObjectClass, UObject* SourceObject=nullptr, int InitSize=0);

	UFUNCTION(BlueprintCallable)
		bool DestroyObjectPool(const UClass* InObjectClass, UObject* SourceObject=nullptr);

	UFUNCTION(BlueprintCallable)
		UObject* RetainObject(const UClass* ObjectClass, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
		bool ReleaseObject(UObject* Object, UObject* SourceObject = nullptr, const UClass* ObjectClass=nullptr);

private:
	FDelegateHandle WorldBeginTearDownHandler;

	TArray<FClassObjectCachePool*> ClassObjectCachePool;
};
