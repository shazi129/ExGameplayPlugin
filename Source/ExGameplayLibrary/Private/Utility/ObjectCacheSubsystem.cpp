#include "Utility/ObjectCacheSubsystem.h"
#include "Kismet/GameplayStatics.h"

void FClassObjectCachePool::InitializePool()
{
	CreateObjects(DesignSize);
}

UObject* FClassObjectCachePool::Retain()
{
	check(CacheObjects.Num() == CacheObjectStatus.Num());

	int ObjectIndex = 0;
	for (; ObjectIndex < CacheObjects.Num(); ObjectIndex++)
	{
		if (CacheObjectStatus[ObjectIndex] == 0)
		{
			break;
		}
	}

	//不够用了, 新增一个
	if (ObjectIndex == CacheObjects.Num())
	{
		CreateObjects(1);
	}

	CacheObjectStatus[ObjectIndex] = 1;
	OnObjectRetain(CacheObjects[ObjectIndex]);
	return  CacheObjects[ObjectIndex];
}

bool FClassObjectCachePool::Release(UObject* Item)
{
	check(CacheObjects.Num() == CacheObjectStatus.Num());

	int ItemIndex = CacheObjects.Find(Item);
	if (CacheObjects.IsValidIndex(ItemIndex))
	{
		CacheObjectStatus[ItemIndex] = 0;
		OnObjectRelease(CacheObjects[ItemIndex]);
	}
	return true;
}

void FActorCachePool::DeinitializePool()
{
	if (!ContextWorld || !ObjectClass)
	{
		return;
	}

	for (UObject* Object : CacheObjects)
	{
		if (AActor* Actor = Cast<AActor>(Object))
		{
			OnObjectDestroy(Actor);
			Actor->Destroy();
		}
	}
	CacheObjects.Empty();
}

void FActorCachePool::CreateObjects(int Size)
{
	if (!ContextWorld || !ObjectClass)
	{
		return;
	}

	for (int i = 0; i < Size; i++)
	{
		AActor* Actor = ContextWorld->SpawnActor(ObjectClass);
		if (Actor)
		{
			CacheObjects.Add(Actor);
			CacheObjectStatus.Add(0);
			OnObjectCreate(Actor);
		}
	}
}

void UObjectCacheSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	WorldBeginTearDownHandler = FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UObjectCacheSubsystem::OnWorldTearingDown);
}

void UObjectCacheSubsystem::Deinitialize()
{
	Super::Deinitialize();
	FWorldDelegates::OnWorldBeginTearDown.Remove(WorldBeginTearDownHandler);

	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool)
		{
			Pool->DeinitializePool();
		}
	}
	ClassObjectCachePool.Empty();
}

void UObjectCacheSubsystem::OnWorldTearingDown(UWorld* World)
{
	for (int i = ClassObjectCachePool.Num() - 1; i >= 0; i--)
	{
		if (ClassObjectCachePool[i] && ClassObjectCachePool[i]->GetWorld() == World)
		{
			ClassObjectCachePool[i]->DeinitializePool();
			ClassObjectCachePool.RemoveAt(i);
		}
	}
}

bool UObjectCacheSubsystem::CreateObjectPool(UScriptStruct* InScriptStruct, UClass* ObjectClass, UWorld* ContextWorld, int DesignSize)
{
	if (!InScriptStruct->IsChildOf(FClassObjectCachePool::StaticStruct()))
	{
		return false;
	}

	//已经存在相同的
	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->GetClass() == ObjectClass)
		{
			return true;
		}
	}

	//申请内存
	const int32 MinAlignment = InScriptStruct->GetMinAlignment();
	const int32 RequiredSize = InScriptStruct->GetStructureSize();
	uint8* Memory = ((uint8*)FMemory::Malloc(FMath::Max(1, RequiredSize), MinAlignment));

	//初始化
	InScriptStruct->InitializeStruct(Memory);
	FClassObjectCachePool* NewPool = (FClassObjectCachePool*)(Memory);
	NewPool->SetObjectClass(ObjectClass);
	NewPool->SetContextWorld(ContextWorld);
	NewPool->SetDesigneSize(DesignSize);

	NewPool->InitializePool();
	ClassObjectCachePool.Add(NewPool);
	return true;
}

bool UObjectCacheSubsystem::DestroyObjectPool(UClass* Class)
{
	if (!Class)
	{
		return false;
	}

	for (int i = 0; i < ClassObjectCachePool.Num(); i++)
	{
		if (ClassObjectCachePool[i]->GetClass() == Class)
		{
			ClassObjectCachePool[i]->DeinitializePool();
			ClassObjectCachePool.RemoveAt(i);
			return true;
		}
	}
	return true;
}

UObject* UObjectCacheSubsystem::RetainObject(UClass* ObjectClass)
{
	if (!ObjectClass)
	{
		return nullptr;
	}

	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->GetClass() == ObjectClass)
		{
			return Pool->Retain();
		}
	}
	return nullptr;
}

bool UObjectCacheSubsystem::ReleaseObject(UObject* Object)
{
	if (!Object)
	{
		return true;
	}

	UClass* ObjectClass = Object->GetClass();
	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->GetClass() == ObjectClass)
		{
			return Pool->Release(Object);
		}
	}
	return false;
}
