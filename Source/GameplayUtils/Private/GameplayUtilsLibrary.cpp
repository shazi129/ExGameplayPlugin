#include "GameplayUtilsLibrary.h"
#include "GameplayUtilsModule.h"
#include "Kismet/KismetSystemLibrary.h"

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

		bool Matched = false;

		//检查Actor的类
		Matched = FilterCondition.ActorClasses.IsEmpty();
		for (auto& ActorClass : FilterCondition.ActorClasses)
		{
			if (Actor->IsA(ActorClass))
			{
				Matched = true;
				break;
			}
		}
		if (!Matched)
		{
			continue;
		}

		//检查组件
		for (auto& RequireComponentClass : FilterCondition.RequireComponentClasses)
		{
			if (Actor->GetComponentByClass(RequireComponentClass) == nullptr)
			{
				Matched = false;
				break;
			}
		}
		if (!Matched)
		{
			continue;
		}

		OutActors.Add(Actor);
	}
}
