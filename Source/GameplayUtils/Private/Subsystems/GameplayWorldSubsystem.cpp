#include "Subsystems/GameplayWorldSubsystem.h"
#include "Macros/SubsystemMacros.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"

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
	Super::Initialize(Collection);
	WorldTearDownHandler = FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UGameplayWorldSubsystem::OnWorldTearingDown);
	WorldInitializedActorsHandler = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UGameplayWorldSubsystem::OnWorldInitializedActors);
}

void UGameplayWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	FWorldDelegates::OnWorldBeginTearDown.Remove(WorldTearDownHandler);
	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitializedActorsHandler);
}

void UGameplayWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	bIsWorldBeginplay = true;
	if (WorldBeginPlayDelegate.IsBound())
	{
		UWorld* World = GetWorld();
		WorldBeginPlayDelegate.Broadcast(World);
	}
}

void UGameplayWorldSubsystem::OnWorldTearingDown(UWorld* World)
{
	if (WorldTeardownDeletage.IsBound())
	{
		WorldTeardownDeletage.Broadcast(World);
	}
}

void UGameplayWorldSubsystem::OnWorldInitializedActors(const UWorld::FActorsInitializedParams& Params)
{
	if (WorldInitializedActorsDeletage.IsBound())
	{
		WorldInitializedActorsDeletage.Broadcast(Params.World);
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

bool UGameplayWorldSubsystem::AddGlobalObject(FName ObjectName, UObject* Object)
{
	auto& ObjectList = GlobalObjectsMap.FindOrAdd(ObjectName);
	ObjectList.Data.AddUnique(Object);
	return false;
}

void UGameplayWorldSubsystem::RemoveGlobalObjects(FName ObjectName)
{
	GlobalObjectsMap.Remove(ObjectName);
}

