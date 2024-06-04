#include "ObjectCache/ObjectCacheSubsystem.h"
#include "ObjectCache/ActorCachePool.h"
#include "GameplayUtilsModule.h"
#include "Macros/SubsystemMacros.h"
#include "Settings/GameplayUtilSettings.h"

UObjectCacheSubsystem* UObjectCacheSubsystem::Get(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogGameplayUtils, UObjectCacheSubsystem, WorldContextObject);
}

bool UObjectCacheSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Outer)
	{
		return false;
	}

	UWorld* World = Outer->GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

void UObjectCacheSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReplicateActorMap.Empty();

	WorldBeginTearDownHandler = FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UObjectCacheSubsystem::OnWorldTearingDown);
	WorldInitializedActorsHandler = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UObjectCacheSubsystem::OnWorldInitializedActors);
}

void UObjectCacheSubsystem::Deinitialize()
{
	Super::Deinitialize();
	FWorldDelegates::OnWorldBeginTearDown.Remove(WorldBeginTearDownHandler);
	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitializedActorsHandler);

	for (UObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool)
		{
			Pool->DeinitializePool();
		}
	}
	ClassObjectCachePool.Empty();
}

void UObjectCacheSubsystem::OnWorldInitializedActors(const UWorld::FActorsInitializedParams& Params)
{
	if (!Params.World || !Params.World->IsGameWorld())
	{
		return;
	}

	ENetMode NetMode = Params.World->GetNetMode();
	if (NetMode == ENetMode::NM_Client || NetMode == ENetMode::NM_Standalone)
	{
		return;
	}

	auto ReplicateActorPtr = ReplicateActorMap.Find(Params.World);
	if (ReplicateActorPtr && ReplicateActorPtr->IsValid())
	{
		return;
	}
	
	UClass* ReplicateActorClass = nullptr;
	auto ReplicateActorClassPtr = GetDefault<UGameplayUtilSettings>()->ObjectCacheReplicateActorClass;
	if (!ReplicateActorClassPtr.IsNull())
	{
		ReplicateActorClass = ReplicateActorClassPtr.LoadSynchronous();
	}
	if (!ReplicateActorClass)
	{
		ReplicateActorClass = AObjectCacheReplicateActor::StaticClass();
	}

	auto GameInstance = GetGameInstance();

	AObjectCacheReplicateActor* ReplicateActor = Params.World->SpawnActor<AObjectCacheReplicateActor>(ReplicateActorClass);
	ReplicateActorMap.Add(Params.World, ReplicateActor);
	
}

void UObjectCacheSubsystem::OnWorldTearingDown(UWorld* World)
{
	if (!World || !World->IsGameWorld())
	{
		return;
	}

	ReplicateActorMap.Remove(World);

	for (int i = ClassObjectCachePool.Num() - 1; i >= 0; i--)
	{
		if (ClassObjectCachePool[i] && ClassObjectCachePool[i]->ContextWorld == World)
		{
			ClassObjectCachePool[i]->DeinitializePool();
			ClassObjectCachePool.RemoveAt(i);
		}
	}
}

bool UObjectCacheSubsystem::CreateObjectPool(const TSubclassOf<UObjectCachePool>& InPoolClass, UClass* InObjectClass, UObject* InSourceObject, int InitSize)
{
	if (!InSourceObject || !InPoolClass || !InObjectClass)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, SourceObject or InScriptStruct or InObjectClass is null"), *FString(__FUNCTION__));
		return false;
	}

	InSourceObject = InSourceObject? InSourceObject : GetWorld();

	//已经存在相同的
	for (UObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->ObjectClass == InObjectClass && Pool->SourceObject == InSourceObject)
		{
			Pool->Reference = Pool->Reference <= 0 ? 1 : Pool->Reference + 1;
			GAMEPLAYUTILS_LOG(Warning, TEXT("%s warning, create dumplicate pool, SouceObject:%s, Type:%s, Reference:%d"), *FString(__FUNCTION__), *GetNameSafe(InSourceObject), *GetNameSafe(InPoolClass), Pool->Reference);
			return true;
		}
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s create %s for %s"), *FString(__FUNCTION__),  *GetNameSafe(InPoolClass), *GetNameSafe(InSourceObject));

	//初始化
	UObjectCachePool* NewPool = NewObject<UObjectCachePool>(InSourceObject, InPoolClass);
	NewPool->SetCacheInfo(InSourceObject, InObjectClass);
	NewPool->InitSize = InitSize;
	NewPool->Reference = 1;

	NewPool->InitializePool();
	ClassObjectCachePool.Add(NewPool);
	return true;
}

bool UObjectCacheSubsystem::DestroyObjectPool(const UClass* InObjectClass)
{
	if (!InObjectClass)
	{
		return false;
	}

	for (int i = 0; i < ClassObjectCachePool.Num(); i++)
	{
		if (ClassObjectCachePool[i]->ObjectClass == InObjectClass)
		{
			ClassObjectCachePool[i]->Reference--;
			if (ClassObjectCachePool[i]->Reference > 0)
			{
				GAMEPLAYUTILS_LOG(Log, TEXT("%s ignore, Object pool has another reference"), *FString(__FUNCTION__));
				return true;
			}
			ClassObjectCachePool[i]->DeinitializePool();
			ClassObjectCachePool.RemoveAt(i);
			return true;
		}
	}
	return true;
	
}

UObject* UObjectCacheSubsystem::RetainObject(const UClass* ObjectClass)
{
	if (!ObjectClass)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s, ObjectClass is null"), *FString(__FUNCTION__));
		return nullptr;
	}

	for (UObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->ObjectClass == ObjectClass)
		{
			return Pool->Retain();
		}
	}
	
	GAMEPLAYUTILS_LOG(Error, TEXT("%s, Cannot find pool of class %s, current size:%d"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass), ClassObjectCachePool.Num());
	return nullptr;
}


AActor* UObjectCacheSubsystem::RetainActor(UClass* ObjectClass, const FTransform& SpawnTransform)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("%s, %s"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass));
	
	if (!ObjectClass)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s, ObjectClass is null"), *FString(__FUNCTION__));
		return nullptr;
	}

	for (UObjectCachePool* Pool : ClassObjectCachePool)
	{
		UActorCachePool* ActorPool = Cast<UActorCachePool>(Pool);
		if (ActorPool && ActorPool->ObjectClass == ObjectClass)
		{
			return ActorPool->RetainActor(SpawnTransform);
		}
	}

	GAMEPLAYUTILS_LOG(Warning, TEXT("%s, Cannot find pool of class %s, using system spawn actor"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass));
	
	//没找到，默认Spawn一个Actor
	if (UWorld* World = GetWorld())
	{
		AActor* Actor = World->SpawnActorDeferred<AActor>(ObjectClass, SpawnTransform);
		if (Actor)
		{
			UGameplayStatics::FinishSpawningActor(Actor, SpawnTransform);

			if (Actor->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
			{
				IObjectPoolInterface::Execute_OnPoolObjectCreate(Actor);
				IObjectPoolInterface::Execute_OnObjectRetainFromPool(Actor);
			}
			return Actor;
		}
	}
	
	return nullptr;
}



AActor* UObjectCacheSubsystem::RetainActorWithDelegate(UClass* ObjectClass, const FTransform& SpawnTransform, const FOnBeforeRetainDelegate& Delegate)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("%s, %s"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass));
	
	if (!ObjectClass)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s, ObjectClass is null"), *FString(__FUNCTION__));
		return nullptr;
	}

	for (auto Pool : ClassObjectCachePool)
	{
		UActorCachePool* ActorPool = Cast<UActorCachePool>(Pool);
		if (ActorPool && ActorPool->ObjectClass == ObjectClass)
		{
			AActor* Actor = ActorPool->RetainActorDeferred();
			Delegate.ExecuteIfBound(Actor);
			return ActorPool->FinishRetainActor(Actor, SpawnTransform);
		}
	}

	GAMEPLAYUTILS_LOG(Warning, TEXT("%s, Cannot find pool of class %s, using system spawn actor"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass));;
	
	//没找到，默认Spawn一个Actor
	if (UWorld* World = GetWorld())
	{
		AActor* Actor = World->SpawnActorDeferred<AActor>(ObjectClass, SpawnTransform);
		if (Actor)
		{
			Delegate.ExecuteIfBound(Actor);
			return UGameplayStatics::FinishSpawningActor(Actor, SpawnTransform);
		}
	}

	return nullptr;
}

bool UObjectCacheSubsystem::ReleaseObject(UObject* Object, const UClass* ObjectClass)
{
	if (!Object)
	{
		return true;
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s, Object: %s"), *FString(__FUNCTION__), *GetNameSafe(Object));;

	ObjectClass = ObjectClass != nullptr ? ObjectClass : Object->GetClass();
	for (UObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->ObjectClass == ObjectClass)
		{
			if (Pool->Release(Object))
			{
				return true;
			}
		}
	}

	if (AActor* Actor = Cast<AActor>(Object))
	{
		if (!Actor->IsPendingKillPending() && Actor->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
		{
			IObjectPoolInterface::Execute_OnPoolObjectRelease(Actor);
			IObjectPoolInterface::Execute_OnPoolObjectDestroy(Actor);
		}
		Actor->Destroy();
	}
	return false;
}

bool UObjectCacheSubsystem::CanWorldUseObjectPool(const UClass* ObjectClass)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("%s, %s"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass));
	
	if (!ObjectClass)
	{
		GAMEPLAYUTILS_LOG(Log, TEXT("%s, ObjectClass is null"), *FString(__FUNCTION__));
		return false;
	}

	for (UObjectCachePool* Pool : ClassObjectCachePool)
	{
		if (Pool && Pool->ObjectClass == ObjectClass)
		{
			return true;
		}
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s, Cannot find pool of class %s, current size:%d"), *FString(__FUNCTION__), *GetNameSafe(ObjectClass), ClassObjectCachePool.Num());
	return false;
}
