#pragma once

#include "CoreMinimal.h"
#include "ObjectCachePool.generated.h"

UINTERFACE(BlueprintType)
class GAMEPLAYUTILS_API UObjectPoolInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEPLAYUTILS_API IObjectPoolInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Cache Pool")
	void OnObjectRetainFromPool();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Cache Pool")
	void OnObjectRetainFromPoolWithTransform(const FTransform& Transform);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPoolObjectCreate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPoolObjectDestroy();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPoolObjectRelease();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectPoolObjectDelegate, UObject*, Object);

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UObjectCachePool : public UObject
{
	friend class UObjectCacheSubsystem;

	GENERATED_BODY()

#pragma region 各种事件的回调
public:
	UPROPERTY(BlueprintAssignable)
	FObjectPoolObjectDelegate ObjectCreateDelegate;

	UPROPERTY(BlueprintAssignable)
	FObjectPoolObjectDelegate ObjectDestroyDelegate;

	UPROPERTY(BlueprintAssignable)
	FObjectPoolObjectDelegate ObjectRetainDelegate;

	UPROPERTY(BlueprintAssignable)
	FObjectPoolObjectDelegate ObjectReleaseDelegate;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "InitializePool"))
	void ReceiveInitializePool();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnObjectCreate"))
	void ReceiveOnObjectCreate(UObject* Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnObjectDestroy"))
	void ReceiveOnObjectDestroy(UObject* Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnObjectRetain"))
	void ReceiveOnObjectRetain(UObject* Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnObjectRelease"))
	void ReceiveOnObjectRelease(UObject* Item);

	virtual void NativeOnObjectCreate(UObject* Item);
	virtual void NativeOnObjectDestroy(UObject* Item);
	virtual void NativeOnObjectRetain(UObject* Item);
	virtual void NativeOnObjectRelease(UObject* Item);

#pragma endregion

protected:
	//创建和销毁对象池
	virtual void InitializePool();
	virtual void DeinitializePool();

	//创建或销毁对象的逻辑
	virtual UObject* CreateObject() { return nullptr; }
	virtual void DestroyObject(UObject* Object) {}

	//从对象池中拿
	virtual UObject* Retain();
	virtual UObject* RetainDeferred();
	virtual void RetainFinish(UObject* Object);

	//回收对象
	virtual bool Release(UObject* Object);
	virtual bool ReleseDeferred(UObject* Object);
	virtual void ReleseFinish(UObject* Object);

	virtual UObject* RetainInternal();

private:
	void SetCacheInfo(UObject* InSourceObject, UClass* InObjectClass)
	{
		SourceObject = InSourceObject;
		ContextWorld = InSourceObject ? InSourceObject->GetWorld() : nullptr;
		ObjectClass = InObjectClass;
	}

protected:
	//类型
	UPROPERTY(EditAnywhere)
	UClass* ObjectClass;

	//初始大小
	UPROPERTY(EditAnywhere)
	int InitSize;

	//这个缓冲池由谁创建
	TWeakObjectPtr<UObject> SourceObject;

	//所有Object都是基于一个World
	UWorld* ContextWorld;

	enum ECachedObjectStatus
	{
		E_Unused, //没在用
		E_Using, //正在用
	};

	//缓存的数据
	struct FCacheObjectItem
	{
		TWeakObjectPtr<UObject> Object;
		ECachedObjectStatus Status;
		void Reset();
	};
	TArray<FCacheObjectItem> CacheObjects;

	//引用计数
	int Reference = 0;
};