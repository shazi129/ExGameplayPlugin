#include "GameplayUtilsLibrary.h"
#include "GameplayUtilsModule.h"
#include "Kismet/KismetSystemLibrary.h"

bool FFilterActorCondition::FilterActorClasses(AActor* Actor) const
{
	if (!Actor) return false;
	if (ActorClasses.IsEmpty())
	{
		return true;
	}
	for (auto& ActorClass : ActorClasses)
	{
		if (Actor->IsA(ActorClass))
		{
			return true;
		}
	}
	return false;
}

bool FFilterActorCondition::FilterExcludeComponentClasses(AActor* Actor) const
{
	if (!Actor) return false;
	for (auto& ExcludeComponentClass : ExcludeComponentClasses)
	{
		if (Actor->GetComponentByClass(ExcludeComponentClass))
		{
			return false;
		}
	}
	return true;
}

bool FFilterActorCondition::FilterRequireComponentClasses(AActor* Actor) const
{
	if (!Actor) return false;
	if (RequireComponentClasses.IsEmpty())
	{
		return true;
	}

	bool HasFound = false;
	for (auto& RequireComponentClass : RequireComponentClasses)
	{
		if (Actor->GetComponentByClass(RequireComponentClass))
		{
			HasFound = true;
			break;
		}
	}
	return HasFound;
}

bool FFilterActorCondition::FilterIgnoreActors(AActor* Actor) const
{
	if (!Actor) return false;
	if (IgnoreActors.IsEmpty() || !IgnoreActors.Contains(Actor))
	{
		return true;
	}
	return false;
}


bool UGameplayUtilsLibrary::ExecCommand(const FString& Command)
{
	if (!GEngine)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, GEngine is Null"), *FString(__FUNCTION__));
		return false;
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s[%s]"), *FString(__FUNCTION__), *Command);

	UWorld* World = nullptr;

	ULocalPlayer* Player = GEngine->GetDebugLocalPlayer();
	if (Player)
	{
		World = Player->GetWorld();
		if (World)
		{
			if (Player->Exec(World, *Command, *GLog))
			{
				return true;
			}
		}
		else
		{
			GAMEPLAYUTILS_LOG(Error, TEXT("%s error, PlayerWorld is Null"), *FString(__FUNCTION__));
			return false;
		}
	}

	if (!World)
	{
		World = GEngine->GetWorld();
	}
	return GEngine->Exec(World, *Command);
}

void UGameplayUtilsLibrary::FilterActors(const TArray<AActor*>& Actors, const FFilterActorCondition FilterCondition, TArray<AActor*>& OutActors)
{
	check(&Actors != &OutActors)

	for (auto& Actor : Actors)
	{
		if (!Actor)
		{
			continue;
		}

		//检查是否ignore
		if (!FilterCondition.FilterIgnoreActors(Actor))
		{
			continue;
		}

		if (!FilterCondition.FilterExcludeComponentClasses(Actor))
		{
			continue;
		}

		if (!FilterCondition.FilterRequireComponentClasses(Actor))
		{
			continue;
		}

		if (!FilterCondition.FilterActorClasses(Actor))
		{
			continue;
		}

		OutActors.Add(Actor);
	}
}

