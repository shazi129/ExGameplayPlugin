#include "Utils/StreamLevelSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"

void UStreamLevelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LevelAddedHandle = FWorldDelegates::LevelAddedToWorld.AddUObject(this, &UStreamLevelSubsystem::OnLevelAdded);
}

void UStreamLevelSubsystem::Deinitialize()
{
	Super::Deinitialize();
	FWorldDelegates::LevelAddedToWorld.Remove(LevelAddedHandle);
}

bool UStreamLevelSubsystem::SpawnActor(UObject* WorldContextObject, UClass* ActorClass, const FString& SubLevelName, FSpawnSubLevelActorDelegate Delegate)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (World && World->IsGameWorld() && World->WorldComposition)
	{
		for (TObjectPtr<ULevelStreaming>& LevelStreaming : World->WorldComposition->TilesStreaming)
		{
			FString LevelName = FPackageName::GetShortName(LevelStreaming->PackageNameToLoad);
			if (LevelName == SubLevelName)
			{
				if (ULevel* Level = LevelStreaming->GetLoadedLevel())
				{
					DoSpawnActor(World, Level, ActorClass, Delegate);
				}
				else
				{
					FSpawnActorDesc* Desc = new (SpawnActorDescCache)FSpawnActorDesc();
					Desc->World = World;
					Desc->ActorClass = ActorClass;
					Desc->SubLevelName = SubLevelName;
					Desc->Delegate = Delegate;
				}
				return true;
			}
		}
	}
	return false;
}

void UStreamLevelSubsystem::OnLevelAdded(ULevel* Level, UWorld* World)
{
	FString LevelPackageFullName = Level->GetPackage()->GetName();
	FString LevelPackageShortName = FPackageName::GetShortName(LevelPackageFullName);
	LevelPackageShortName.RemoveFromStart(World->StreamingLevelsPrefix);

	for (int DescIndex = SpawnActorDescCache.Num() - 1; DescIndex >= 0; DescIndex--)
	{
		FSpawnActorDesc& Desc = SpawnActorDescCache[DescIndex];
		if (LevelPackageShortName == Desc.SubLevelName)
		{
			DoSpawnActor(World, Level, Desc.ActorClass, Desc.Delegate);
			SpawnActorDescCache.RemoveAt(DescIndex);
		}
	}
}

AActor* UStreamLevelSubsystem::DoSpawnActor(UWorld* World, ULevel* Level, UClass* ActorClass, FSpawnSubLevelActorDelegate& Delegate)
{
	AActor* Actor = World->SpawnActor(ActorClass);
	Actor->Rename(nullptr, Level);
	Level->Actors.Add(Actor);
	Delegate.Execute(Actor);
	return Actor;
}