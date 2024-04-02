#pragma once

#include "ObjectCache/ObjectCachePool.h"
#include "ActorCachePool.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UActorCachePool : public UObjectCachePool
{
	GENERATED_BODY()

public:
	UActorCachePool();

	virtual UObject* CreateObject() override;
	virtual void DestroyObject(UObject* Object) override;

	virtual UObject* Retain() override;
	virtual bool Release(UObject* Item) override;

	virtual AActor* RetainActor(const FTransform& Transform = FTransform::Identity);
	virtual AActor* RetainActorDeferred(const FTransform& Transform = FTransform::Identity);
	virtual AActor* FinishRetainActor(AActor* Actor, const FTransform& Transform = FTransform::Identity);

protected:
	virtual void SetActorPoolState(AActor* Actor, bool bInPool);

public:
	//处于回收状态时，Actor的位置
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform ResetTransform;

	//自动显隐，Release时隐藏，Retain时显示
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bAutoVisible;

protected:
	static void InitializeFromCDO(UObject* Object);
};
