#include "ObjectCacheSubsystem.h"
#include "GameplayUtilsModule.h"

void FClassObjectCachePool::FCacheObjectItem::Reset()
{
	Object = nullptr;
	Status = 0;
}

bool FClassObjectCachePool::FCacheObjectItem::IsValid()
{
	return Object.IsValid();
}

void FClassObjectCachePool::InitializePool()
{
	for (int i = 0; i < DesignSize; i++)
	{
		if (UObject* Object = CreateObject())
		{
			FCacheObjectItem* Item = new (CacheObjects)FCacheObjectItem();
			Item->Object = Object;
			Item->Status = 0;
		}
	}
}

UObject* FClassObjectCachePool::Retain()
{
	int StartIndex = 0;
	int EndIndex = CacheObjects.Num() - 1;

	while (StartIndex <= EndIndex)
	{
		//找到一个合法的
		if (CacheObjects[StartIndex].IsValid())
		{
			if (CacheObjects[StartIndex].Status == 0) //可以返回了
			{
				break;
			}
			else
			{
				StartIndex++;
			}
		}
		//遇到一个不合法的，把它交换到最后面
		else
		{
			if (StartIndex != EndIndex)
			{
				CacheObjects[StartIndex] = CacheObjects[EndIndex];
			}
			CacheObjects[EndIndex].Reset();
			EndIndex--;
		}
	}

	//没找到
	if (StartIndex > EndIndex)
	{
		if (StartIndex == CacheObjects.Num()) //要创建新的了
		{
			new (CacheObjects)FCacheObjectItem();
		}

		CacheObjects[StartIndex].Object = CreateObject();
		CacheObjects[StartIndex].Status = 0;
	}

	OnObjectRetain(CacheObjects[StartIndex].Object.Get());
	CacheObjects[StartIndex].Status = 1;

	return  CacheObjects[StartIndex].Object.Get();
}

bool FClassObjectCachePool::Release(UObject* Object)
{
	if (Object == nullptr)
	{
		return false;
	}

	for (FCacheObjectItem& Item : CacheObjects)
	{
		if (!Item.IsValid())
		{
			Item.Reset();
		}
		else if (Item.Object == Object)
		{
			OnObjectRelease(Item.Object.Get());
			Item.Status = 0;
		}
	}
	return true;
}

void FActorCachePool::DeinitializePool()
{
	//World没有了，里面的Actor自己就没有了，不用特殊处理

	if (ContextWorld)
	{
		for (FCacheObjectItem& Item : CacheObjects)
		{
			if (Item.IsValid())
			{
				if (AActor* Actor = Cast<AActor>(Item.Object.Get()))
				{
					OnObjectDestroy(Actor);
					Actor->Destroy();
				}
			}
		}
	}
	CacheObjects.Empty();
}

UObject* FActorCachePool::CreateObject()
{
	if (!ContextWorld || !ObjectClass)
	{
		return nullptr;
	}

	AActor* Actor = ContextWorld->SpawnActor(ObjectClass);
	if (Actor)
	{
		OnObjectCreate(Actor);
	}
	return Actor;
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
			ClassObjectCachePool[i]->SetContextWorld(nullptr);
			ClassObjectCachePool[i]->DeinitializePool();
			ClassObjectCachePool.RemoveAt(i);
		}
	}
}

bool UObjectCacheSubsystem::CreateObjectPool(UScriptStruct* InScriptStruct, UClass* ObjectClass, UWorld* ContextWorld, int DesignSize)
{
	if (!InScriptStruct || !InScriptStruct->IsChildOf(FClassObjectCachePool::StaticStruct()))
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, %s is not a FClassObjectCachePool"), *FString(__FUNCTION__), *GetNameSafe(InScriptStruct));
		return false;
	}

	//已经存在相同的
	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->GetClass() == ObjectClass)
		{
			Pool->ReferenceCount = Pool->ReferenceCount <= 0 ? 1 : Pool->ReferenceCount + 1;
			GAMEPLAYUTILS_LOG(Log, TEXT("%s exist same pool for type: %s, reference count:%d"), *FString(__FUNCTION__), *GetNameSafe(InScriptStruct), Pool->ReferenceCount);
			return true;
		}
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s for %s, wrapper:"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass), *GetNameSafe(InScriptStruct));

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
	NewPool->ReferenceCount = 1;

	NewPool->InitializePool();
	ClassObjectCachePool.Add(NewPool);
	return true;
}

bool UObjectCacheSubsystem::DestroyObjectPool(UClass* Class, bool OnlyClearItems)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("%s for: %s, %d, current size:%d"), *FString(__FUNCTION__), *GetNameSafe(Class), OnlyClearItems, ClassObjectCachePool.Num());
	if (!Class)
	{
		return false;
	}

	for (int i = 0; i < ClassObjectCachePool.Num(); i++)
	{
		if (ClassObjectCachePool[i]->GetClass() == Class)
		{
			ClassObjectCachePool[i]->ReferenceCount--;
			if (ClassObjectCachePool[i]->ReferenceCount > 0)
			{
				GAMEPLAYUTILS_LOG(Log, TEXT("%s ignore, Object pool has another reference"), *FString(__FUNCTION__));
				return true;
			}

			ClassObjectCachePool[i]->DeinitializePool();
			if (!OnlyClearItems)
			{
				ClassObjectCachePool.RemoveAt(i);
			}
			return true;
		}
	}
	return true;
}

UObject* UObjectCacheSubsystem::RetainObject(UClass* ObjectClass)
{
	if (!ObjectClass)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s, ObjectClass is null"), *FString(__FUNCTION__));
		return nullptr;
	}

	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->GetClass() == ObjectClass)
		{
			return Pool->Retain();
		}
	}

	GAMEPLAYUTILS_LOG(Error, TEXT("%s, Cannot find pool of class %s, current size:%d"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass), ClassObjectCachePool.Num());
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
