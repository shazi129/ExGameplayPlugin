#include "FunctionLibraries/GameplayUtilsLibrary.h"
#include "GameplayUtilsModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerState.h"
#include "FunctionLibraries/PathHelperLibrary.h"

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

APlayerState* UGameplayUtilsLibrary::GetPlayerStateByActor(AActor* Actor)
{
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->GetPlayerState();
	}
	return nullptr;
}

UActorComponent* UGameplayUtilsLibrary::GetComponentByTag(AActor* Actor, const FName& Tag)
{
	if (Actor)
	{
		TArray<UActorComponent*> Components = Actor->GetComponentsByTag(UActorComponent::StaticClass(), Tag);
		if (Components.Num() > 0)
		{
			return Components[0];
		}
	}
	return nullptr;
}

UActorComponent* UGameplayUtilsLibrary::GetComponentByClass(AActor* Actor, TSubclassOf<UActorComponent> ComponentClass)
{
	if (Actor && ComponentClass)
	{
		return Actor->GetComponentByClass(ComponentClass);
	}
	return nullptr;
}

FString UGameplayUtilsLibrary::GetNameSafe(const UObject* Object)
{
	return ::GetNameSafe(Object);
}

FString UGameplayUtilsLibrary::GetContextWorldName(const UObject* Object)
{
	if (!Object)
	{
		return "None";
	}

	UWorld* ContextWorld = Object->GetWorld();
	if (!ContextWorld)
	{
		return "None";
	}

	if (UPackage* Package = ContextWorld->GetPackage())
	{
		FString PackageShortName = FPackageName::GetShortName(Package->GetName());
		PackageShortName.RemoveFromStart(ContextWorld->StreamingLevelsPrefix);
		return PackageShortName;
	}
	return "None";
}

UObject* UGameplayUtilsLibrary::CopyObject(UObject* TemplateObject, UObject* Outer, FName Name)
{
	if (TemplateObject)
	{
		FStaticConstructObjectParameters Params(TemplateObject->GetClass());
		Params.Outer = Outer;
		Params.Name = Name;
		Params.Template = TemplateObject;
		Params.bCopyTransientsFromClassDefaults = true;
		return StaticConstructObject_Internal(Params);
	}
	return nullptr;
}

TArray<UActorComponent*> UGameplayUtilsLibrary::GetComponentsByTickEnable(AActor* Actor, bool Enabled)
{
	TArray<UActorComponent*> Components;
	if (!Actor)
	{
		return MoveTemp(Components);
	}

	const TSet<UActorComponent*>&  ActorComponents = Actor->GetComponents();
	for (UActorComponent* ActorComponent : ActorComponents)
	{
		if (ActorComponent->IsComponentTickEnabled() == Enabled)
		{
			Components.Add(ActorComponent);
		}
	}

	return MoveTemp(Components);
}

void UGameplayUtilsLibrary::SetComponentsTickEnable(TArray<UActorComponent*>& Components, bool Enable)
{
	for (UActorComponent* Component : Components)
	{
		if (Component)
		{
			Component->SetComponentTickEnabled(Enable);
		}
	}
}

bool UGameplayUtilsLibrary::IsCurrentWorld(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld>& TargetWorld)
{
	if (!WorldContextObject || TargetWorld.IsNull())
	{
		return false;
	}

	if (UWorld* World = WorldContextObject->GetWorld())
	{
		FString TargetWorldPath = TargetWorld.GetLongPackageName();
		FString CurrentWorldPath = UPathHelperLibrary::GetPackageFullName(World);
		return TargetWorldPath == CurrentWorldPath;
	}

	return false;
}

bool UGameplayUtilsLibrary::IsGameWorld(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return false;
	}

	if (World->GetName().Equals(FString("Untitled")))
	{
		return false;
	}

	return true;
}

