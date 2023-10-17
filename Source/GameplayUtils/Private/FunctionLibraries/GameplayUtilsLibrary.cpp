#include "FunctionLibraries/GameplayUtilsLibrary.h"
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

bool UGameplayUtilsLibrary::CheckExecNetMode(const UObject* WorldContextObject, EExecNetMode ExecNetMode)
{
	if (!WorldContextObject)
	{
		return false;
	}
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		ENetMode NetMode = World->GetNetMode();
		bool InDedicatedServer = NetMode == ENetMode::NM_DedicatedServer;
		if (ExecNetMode == EExecNetMode::E_Client && InDedicatedServer)
		{
			return false;
		}
		else if (ExecNetMode == EExecNetMode::E_Server && !InDedicatedServer)
		{
			return false;
		}
		return true;
	}
	return false;
}

