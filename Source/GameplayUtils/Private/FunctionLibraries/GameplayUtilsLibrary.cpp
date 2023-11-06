#include "FunctionLibraries/GameplayUtilsLibrary.h"
#include "GameplayUtilsModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerState.h"

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
		if (ExecNetMode == EExecNetMode::E_Server)
		{
			return NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer  || NetMode == ENetMode::NM_Standalone;
		}
		else if (ExecNetMode == EExecNetMode::E_Client)
		{
			return NetMode == ENetMode::NM_Client || NetMode == ENetMode::NM_ListenServer || NetMode == ENetMode::NM_Standalone;
		}
		else if (ExecNetMode == EExecNetMode::E_ExactClient)
		{
			return NetMode == ENetMode::NM_Client;
		}
		else if (ExecNetMode == EExecNetMode::E_ExactServer)
		{
			return NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer;
		}
		return true;
	}
	return false;
}

bool UGameplayUtilsLibrary::IsLocalControled(AActor* Actor)
{
	if (!Actor) return false;

	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		return false;
	}

	const ENetMode NetMode = World->GetNetMode();
	if (NetMode == NM_Standalone)
	{
		return true;
	}

	ENetRole LocalRole = Actor->GetLocalRole();
	if (NetMode == NM_Client && LocalRole == ROLE_AutonomousProxy)
	{
		return true;
	}

	ENetRole RemoteRole = Actor->GetRemoteRole();
	if (RemoteRole != ROLE_AutonomousProxy && LocalRole == ROLE_Authority)
	{
		return true;
	}

	return false;
}

UObject* UGameplayUtilsLibrary::GetDefaultObject(UClass* ObjectClass)
{
	if (ObjectClass == nullptr)
	{
		return nullptr;
	}
	return ObjectClass->GetDefaultObject();
}

APawn* UGameplayUtilsLibrary::GetPawnByPlayerState(APlayerState* PlayerState)
{
	if (PlayerState)
	{
		if (AController* Controller = PlayerState->GetOwner<AController>())
		{
			return Controller->GetPawn();
		}
	}
	return nullptr;
}

