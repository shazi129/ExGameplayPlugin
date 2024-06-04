#include "ObjectCache/ActorCachePool.h"
#include "Kismet/GameplayStatics.h"

UActorCachePool::UActorCachePool()
	: bAutoVisible(true)
{

}

UObject* UActorCachePool::CreateObject()
{
	if (!ContextWorld || !ObjectClass)
	{
		return nullptr;
	}

	AActor* Actor = ContextWorld->SpawnActorDeferred<AActor>(ObjectClass, ResetTransform);
	if (Actor)
	{
	
		Actor->FinishSpawning(ResetTransform);
		
		//初始不可同步
		Actor->PrimaryActorTick.bStartWithTickEnabled = false;

		//始终网络相关
		Actor->bAlwaysRelevant = true;

		SetActorPoolState(Actor, true);

		if (ObjectCreateDelegate.IsBound())
		{
			ObjectCreateDelegate.Broadcast(Actor);
		}

		NativeOnObjectCreate(Actor);
	}
	return Actor;
}

void UActorCachePool::DestroyObject(UObject* Object)
{
	if (AActor* Actor = Cast<AActor>(Object))
	{
		if (ObjectDestroyDelegate.IsBound())
		{
			ObjectDestroyDelegate.Broadcast(Actor);
		}
		NativeOnObjectDestroy(Actor);
		Actor->Destroy();
	}
}

UObject* UActorCachePool::Retain()
{
	return RetainActor(FTransform());
}

bool UActorCachePool::Release(UObject* Item)
{
	AActor* PoolActor = Cast<AActor>(Item);
	if (!PoolActor || PoolActor->IsPendingKillPending())
	{
		return false;
	}

	if (ReleseDeferred(Item))
	{
		UE_LOG(LogTemp, Log, TEXT("%s, %s"), *FString(__FUNCTION__), *GetNameSafe(Item));
		
		PoolActor->SetActorTransform(ResetTransform, false, nullptr, ETeleportType::ResetPhysics);
		SetActorPoolState(PoolActor, true);

		ReleseFinish(Item);
		return true;
	}
	return false;
}

AActor* UActorCachePool::RetainActor(const FTransform& Transform)
{
	AActor* Actor = RetainActorDeferred(Transform);
	return FinishRetainActor(Actor, Transform);
}

AActor* UActorCachePool::RetainActorDeferred(const FTransform& Transform)
{
	AActor* Actor = Cast<AActor>(RetainDeferred());
	UE_LOG(LogTemp, Log, TEXT("%s, %s"), *FString(__FUNCTION__), *GetNameSafe(Actor))
	return Actor;
}

AActor* UActorCachePool::FinishRetainActor(AActor* Actor, const FTransform& Transform)
{
	if (Actor)
	{
		UE_LOG(LogTemp, Log, TEXT("%s, %s"), *FString(__FUNCTION__), *GetNameSafe(Actor));

		Actor->SetActorTransform(Transform, false, nullptr, ETeleportType::ResetPhysics);
		SetActorPoolState(Actor, false);

		RetainFinish(Actor);

		if (Actor->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
		{
			IObjectPoolInterface::Execute_OnObjectRetainFromPoolWithTransform(Actor, Transform);
		}
	}
	return Actor;
}

void UActorCachePool::SetActorPoolState(AActor* Actor, bool bInPool)
{
	if (!Actor)
	{
		return;
	}

	//是否可见
	if (bAutoVisible)
	{
		Actor->SetActorHiddenInGame(bInPool);
	}
	
	//是否可tick
	Actor->SetActorTickEnabled(!bInPool);

	//是否可以网络同步
	if (Actor->HasAuthority())
	{
		ENetDormancy NetDormancy = bInPool ? ENetDormancy::DORM_Initial : ENetDormancy::DORM_Awake;
		
		Actor->FlushNetDormancy();
		Actor->SetNetDormancy(NetDormancy);
	}
}


// 使用Object的CDO对象尝试重置变量，TODO:没有考虑C++变量 
void UActorCachePool::InitializeFromCDO(UObject* Object)
{
	if (!Object) return;
	const UClass* ObjectClass = Object->GetClass();
	if (!ObjectClass)
	{
		return;
	}
	UObject* Default = ObjectClass->GetDefaultObject(true);
	for (TFieldIterator<FProperty> It(ObjectClass); It; ++It)
	{
		const FProperty* Property = *It;
		const void* DefaultPropertyValue = Property->ContainerPtrToValuePtr<void>(Default);
		void* DestPropertyValue = Property->ContainerPtrToValuePtr<void>(Object);
		UE_LOG(LogTemp, Log, TEXT("====================================== Class:%s, Property:%s"), *GetNameSafe(ObjectClass), *Property->GetName());
		Property->CopyCompleteValue(DestPropertyValue, DefaultPropertyValue);
	}
}