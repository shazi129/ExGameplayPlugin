#include "ObjectCacheSubsystem.h"
#include "GameplayUtilsModule.h"
#include "Macros/SubsystemMacros.h"

void FClassObjectCachePool::FCacheObjectItem::Reset()
{
	Object = nullptr;
	Status = 0;
}

void FClassObjectCachePool::InitializePool()
{
	for (int i = 0; i < InitSize; i++)
	{
		if (UObject* Object = CreateObject())
		{
			FCacheObjectItem* Item = new (CacheObjects)FCacheObjectItem();
			Item->Object = Object;
			Item->Status = 0;
		}
	}
}

void FClassObjectCachePool::DeinitializePool()
{
	for (FCacheObjectItem& Item : CacheObjects)
	{
		if (Item.Object.IsValid())
		{
			DestroyObject(Item.Object.Get());
		}
	}
	CacheObjects.Empty();
}

UObject* FClassObjectCachePool::Retain()
{
	int StartIndex = 0;
	int EndIndex = CacheObjects.Num() - 1;

	while (StartIndex <= EndIndex)
	{
		//找到一个合法的
		if (CacheObjects[StartIndex].Object.IsValid())
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
		if (!Item.Object.IsValid())
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

void FActorCachePool::DestroyObject(UObject* Object)
{
	if (AActor* Actor = Cast<AActor>(Object))
	{
		OnObjectDestroy(Actor);
		Actor->Destroy();
	}
}

UObjectCacheSubsystem* UObjectCacheSubsystem::Get(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogGameplayUtils, UObjectCacheSubsystem, WorldContextObject);
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
		if (ClassObjectCachePool[i] && ClassObjectCachePool[i]->ContextWorld == World)
		{
			ClassObjectCachePool[i]->DeinitializePool();
			ClassObjectCachePool.RemoveAt(i);
		}
	}
}

bool UObjectCacheSubsystem::CreateObjectPool(const UScriptStruct* InScriptStruct, UClass* InObjectClass, UObject* InSourceObject, int InitSize)
{
	if (!InSourceObject || !InScriptStruct || !InObjectClass)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, SourceObject or InScriptStruct or InObjectClass is null"), *FString(__FUNCTION__));
		return false;
	}

	if (!InScriptStruct->IsChildOf(FClassObjectCachePool::StaticStruct()))
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, %s is not a FClassObjectCachePool"), *FString(__FUNCTION__), *GetNameSafe(InScriptStruct));
		return false;
	}

	InSourceObject = InSourceObject? InSourceObject : GetWorld();

	//已经存在相同的
	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->ObjectClass == InObjectClass && Pool->SourceObject == InSourceObject)
		{
			Pool->Reference = Pool->Reference <= 0 ? 1 : Pool->Reference + 1;
			GAMEPLAYUTILS_LOG(Warning, TEXT("%s warning, create dumplicate pool, SouceObject:%s, Type:%s, Reference:%d"), *FString(__FUNCTION__), *GetNameSafe(InSourceObject), *GetNameSafe(InScriptStruct), Pool->Reference);
			return true;
		}
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s create %s for %s"), *FString(__FUNCTION__),  *GetNameSafe(InScriptStruct), *GetNameSafe(InSourceObject));

	//申请内存
	const int32 MinAlignment = InScriptStruct->GetMinAlignment();
	const int32 RequiredSize = InScriptStruct->GetStructureSize();
	uint8* Memory = ((uint8*)FMemory::Malloc(FMath::Max(1, RequiredSize), MinAlignment));

	//初始化
	InScriptStruct->InitializeStruct(Memory);
	FClassObjectCachePool* NewPool = (FClassObjectCachePool*)(Memory);
	NewPool->SetCacheInfo(InSourceObject, InObjectClass);
	NewPool->InitSize = InitSize;
	NewPool->Reference = 1;

	NewPool->InitializePool();
	ClassObjectCachePool.Add(NewPool);
	return true;
}

bool UObjectCacheSubsystem::DestroyObjectPool(const UClass* InObjectClass, UObject* SourceObject)
{
	if (!SourceObject || !InObjectClass)
	{
		return false;
	}

	SourceObject = SourceObject ? SourceObject : GetWorld();

	for (int i = 0; i < ClassObjectCachePool.Num(); i++)
	{
		if (ClassObjectCachePool[i]->ObjectClass == InObjectClass && ClassObjectCachePool[i]->SourceObject == SourceObject)
		{
			ClassObjectCachePool[i]->Reference--;
			if (ClassObjectCachePool[i]->Reference > 0)
			{
				GAMEPLAYUTILS_LOG(Log, TEXT("%s ignore, Object pool has another reference"), *FString(__FUNCTION__));
				return true;
			}
			ClassObjectCachePool[i]->DeinitializePool();
			FMemory::Free(ClassObjectCachePool[i]);
			return true;
		}
	}
	return true;
	
}

UObject* UObjectCacheSubsystem::RetainObject(const UClass* ObjectClass, UObject* SourceObject)
{
	if (!ObjectClass)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s, ObjectClass is null"), *FString(__FUNCTION__));
		return nullptr;
	}

	SourceObject = SourceObject ? SourceObject : GetWorld();

	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->ObjectClass == ObjectClass && Pool->SourceObject == SourceObject)
		{
			return Pool->Retain();
		}
	}

	GAMEPLAYUTILS_LOG(Error, TEXT("%s, Cannot find pool of class %s, current size:%d"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass), ClassObjectCachePool.Num());
	return nullptr;
}

bool UObjectCacheSubsystem::ReleaseObject(UObject* Object, UObject* SourceObject, const UClass* ObjectClass)
{
	if (!Object)
	{
		return true;
	}

	SourceObject = SourceObject ? SourceObject : GetWorld();

	ObjectClass = ObjectClass != nullptr ? ObjectClass : Object->GetClass();
	for (FClassObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->ObjectClass == ObjectClass && Pool->SourceObject == SourceObject)
		{
			return Pool->Release(Object);
		}
	}
	return false;
}
