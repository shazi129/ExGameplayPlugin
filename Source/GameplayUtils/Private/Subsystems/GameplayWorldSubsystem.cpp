#include "Subsystems/GameplayWorldSubsystem.h"
#include "Macros/SubsystemMacros.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"
#include "Subsystems/GameplayInstanceSubsystem.h"

void FGameplayURL::Parse(FURL URL)
{
	Protocol = URL.Protocol;
	Host = URL.Host;
	Port = URL.Port;
	Map = URL.Map;

	OptionMap.Empty();

	TArray<FString> OpKV;
	for (auto& Option : URL.Op)
	{
		OpKV.Reserve(2);
		int Num = Option.ParseIntoArray(OpKV, TEXT("="));
		if (Num == 2)
		{
			OptionMap.FindOrAdd(OpKV[0]) = OpKV[1];
		}
	}
}

UGameplayWorldSubsystem* UGameplayWorldSubsystem::Get(const UObject* WorldContextObject)
{
	GET_WORLD_SUBSYSTEM(LogGameplayUtils, UGameplayWorldSubsystem, WorldContextObject);
}

bool UGameplayWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!UGameplayUtilsLibrary::IsGameWorld(Outer))
	{
		return false;
	}
	return Super::ShouldCreateSubsystem(Outer);
}

void UGameplayWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UGameplayWorldSubsystem::Initialize %s"), *GetNameSafe(GetWorld()));

	Super::Initialize(Collection);

	FWorldContext*  WorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (WorldContext)
	{
		RemoteURL.Parse(WorldContext->LastRemoteURL);
	}
}

void UGameplayWorldSubsystem::Deinitialize()
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UGameplayWorldSubsystem::Deinitialize"), *GetNameSafe(GetWorld()));

	Super::Deinitialize();
}

void UGameplayWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UGameplayWorldSubsystem::OnWorldBeginPlay %s"), *GetNameSafe(&InWorld));

	Super::OnWorldBeginPlay(InWorld);
	if (UGameplayInstanceSubsystem* InstanceSubsystem = UGameplayInstanceSubsystem::Get(this))
	{
		if (InstanceSubsystem->WorldBeginPlayDelegate.IsBound())
		{
			InstanceSubsystem->WorldBeginPlayDelegate.Broadcast(&InWorld);
		}
	}
}

UObject* UGameplayWorldSubsystem::GetGlobalObject(FName ObjectName)
{
	if (auto ObjectList = GlobalObjectsMap.Find(ObjectName))
	{
		for (auto& Object : ObjectList->Data)
		{
			if (UGameplayUtilsLibrary::IsValid(Object))
			{
				return Object;
			}
		}
	}
	return nullptr;
}

void UGameplayWorldSubsystem::GetGlobalObjectForDelegate(FName ObjectName, FObjectDynamicDelegate Delegate)
{
	if (UObject* Object = GetGlobalObject(ObjectName))
	{
		Delegate.ExecuteIfBound(Object);
		return;
	}
	GlobalObjectGetDelegateMap.FindOrAdd(ObjectName).Add(Delegate);
}

TArray<UObject*> UGameplayWorldSubsystem::GetGlobalObjectList(FName ObjectName)
{
	TArray<UObject*> Result;
	if (auto ObjectListPtr = GlobalObjectsMap.Find(ObjectName))
	{
		for (auto& Object : ObjectListPtr->Data)
		{
			if (UGameplayUtilsLibrary::IsValid(Object))
			{
				Result.AddUnique(Object);
			}
		}
	}
	return MoveTemp(Result);
}

TArray<UObject*> UGameplayWorldSubsystem::GetGlobalObjectListByClass(UClass* ObjectClass)
{
	TArray<UObject*> Result;
	if (!ObjectClass)
	{
		return MoveTemp(Result);
	}

	for (auto& ObjectsItem : GlobalObjectsMap)
	{
		for (auto& ObjectPtr : ObjectsItem.Value.Data)
		{
			if (UGameplayUtilsLibrary::IsValid(ObjectPtr) && ObjectPtr->IsA(ObjectClass))
			{
				Result.AddUnique(ObjectPtr);
			}
		}
	}

	return MoveTemp(Result);
}

bool UGameplayWorldSubsystem::AddGlobalObject(FName ObjectName, UObject* Object)
{
	if (ObjectName.IsValid() && Object)
	{
		auto& ObjectList = GlobalObjectsMap.FindOrAdd(ObjectName);
		ObjectList.Data.AddUnique(Object);

		if (auto* Delegate = GlobalObjectGetDelegateMap.Find(ObjectName))
		{
			if (Delegate->IsBound())
			{
				Delegate->Broadcast(Object);
			}
			GlobalObjectGetDelegateMap.Remove(ObjectName);
		}

		return true;
	}
	return false;
}

void UGameplayWorldSubsystem::RemoveGlobalObjects(FName ObjectName)
{
	GlobalObjectsMap.Remove(ObjectName);
}

FGameplayURL UGameplayWorldSubsystem::GetRemoteURL()
{
	return RemoteURL;
}
