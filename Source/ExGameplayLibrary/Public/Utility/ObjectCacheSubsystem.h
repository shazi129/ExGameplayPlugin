#pragma once

#include "CoreMinimal.h"
#include "ExMacros.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectCacheSubsystem.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYLIBRARY_API FClassObjectCachePool
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
struct EXGAMEPLAYLIBRARY_API FStructObjectCachePool
{
	GENERATED_BODY()

protected:
	//类型
	UScriptStruct* ScriptStruct;
	//内存
	uint8* Memory;
	//当前大小
	int ItemSize;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYLIBRARY_API FActorCachePool : public FClassObjectCachePool
{
	GENERATED_BODY()
public:
	FActorCachePool() {};
	virtual void DeinitializePool() override;
	virtual UObject* CreateObject();
};

UCLASS(BlueprintType)
class EXGAMEPLAYLIBRARY_API UObjectCacheSubsystem : public UGameInstanceSubsystem
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
		UObject* RetainObject(UClass* ObjectClass);

	UFUNCTION(BlueprintCallable)
		bool ReleaseObject(UObject* Object);

private:
	FDelegateHandle WorldBeginTearDownHandler;

	TArray<FClassObjectCachePool*> ClassObjectCachePool;

	UPROPERTY()
		TArray<FStructObjectCachePool> StructObjectCachePool;
};
