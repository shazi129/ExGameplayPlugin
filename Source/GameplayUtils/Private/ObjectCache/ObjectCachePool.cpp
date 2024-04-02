#include "ObjectCache/ObjectCachePool.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"

void UObjectCachePool::FCacheObjectItem::Reset()
{
	Object = nullptr;
	Status = ECachedObjectStatus::E_Unused;
}

void UObjectCachePool::NativeOnObjectCreate(UObject* Item)
{
	ReceiveOnObjectCreate(Item);
	if (ObjectCreateDelegate.IsBound())
	{
		ObjectCreateDelegate.Broadcast(Item);
	}

	if (Item->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
	{
		IObjectPoolInterface::Execute_OnPoolObjectCreate(Item);
	}
}

void UObjectCachePool::NativeOnObjectDestroy(UObject* Item)
{
	ReceiveOnObjectDestroy(Item);
	if (ObjectDestroyDelegate.IsBound())
	{
		ObjectDestroyDelegate.Broadcast(Item);
	}

	if (Item->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
	{
		IObjectPoolInterface::Execute_OnPoolObjectDestroy(Item);
	}
}
void UObjectCachePool::NativeOnObjectRetain(UObject* Item)
{
	ReceiveOnObjectRetain(Item);

	if (ObjectRetainDelegate.IsBound())
	{
		ObjectRetainDelegate.Broadcast(Item);
	}

	if (Item->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
	{
		IObjectPoolInterface::Execute_OnObjectRetainFromPool(Item);
	}
}
void UObjectCachePool::NativeOnObjectRelease(UObject* Item)
{
	ReceiveOnObjectRelease(Item);
	if (ObjectReleaseDelegate.IsBound())
	{
		ObjectReleaseDelegate.Broadcast(Item);
	}

	if (Item->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
	{
		IObjectPoolInterface::Execute_OnPoolObjectRelease(Item);
	}
}

void UObjectCachePool::InitializePool()
{
	ReceiveInitializePool();

	for (int i = 0; i < InitSize; i++)
	{
		if (UObject* Object = CreateObject())
		{
			FCacheObjectItem* Item = new (CacheObjects)FCacheObjectItem();
			Item->Object = Object;
			Item->Status = ECachedObjectStatus::E_Unused;
		}
	}
}

void UObjectCachePool::DeinitializePool()
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


UObject* UObjectCachePool::Retain()
{
	auto Object = RetainDeferred();
	RetainFinish(Object);
	return Object;
}

UObject* UObjectCachePool::RetainDeferred()
{
	return RetainInternal();
}

void UObjectCachePool::RetainFinish(UObject* Object)
{
	if (Object)
	{
		NativeOnObjectRetain(Object);
	}
}

UObject* UObjectCachePool::RetainInternal()
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
		CacheObjects[StartIndex].Status = ECachedObjectStatus::E_Unused;
	}

	CacheObjects[StartIndex].Status = ECachedObjectStatus::E_Using;

	return  CacheObjects[StartIndex].Object.Get();
}

bool UObjectCachePool::Release(UObject* Object)
{
	if (ReleseDeferred(Object))
	{
		ReleseFinish(Object);
	}

	return true;
}

bool UObjectCachePool::ReleseDeferred(UObject* Object)
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
			Item.Status = ECachedObjectStatus::E_Unused;
			return true;
		}
	}

	return false;
}

void UObjectCachePool::ReleseFinish(UObject* Object)
{
	if (Object)
	{
		NativeOnObjectRelease(Object);
	}
}
