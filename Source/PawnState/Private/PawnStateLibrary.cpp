#include "PawnStateLibrary.h"
#include "PawnStateSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "PawnStateModule.h"
#include "ExGameplayLibrary.h"


UPawnStateComponent* UPawnStateLibrary::GetLocalPawnStateComponent(const UObject* WorldContextObject)
{
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(WorldContextObject, 0))
	{
		return  UPawnStateLibrary::GetPawnStateComponent(Pawn);
	}
	return nullptr;
}

UPawnStateComponent* UPawnStateLibrary::GetPawnStateComponent(AActor* Actor)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}
	UPawnStateComponent* Component = Cast<UPawnStateComponent>(Actor->GetComponentByClass(UPawnStateComponent::StaticClass()));
	return Component;
}

bool UPawnStateLibrary::HasPawnStateTag(AActor* Actor, FGameplayTag PawnStateTag)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->HasPawnStateTag(PawnStateTag);
	}

	PAWNSTATE_LOG(Error, TEXT("%s error, cannot get pawn state component for actor[%s]"), *FString(__FUNCTION__), *GetNameSafe(Actor));
	return false;
}

bool UPawnStateLibrary::CanEnterPawnStateAsset(AActor* Actor, const UPawnStateAsset* Asset)
{
	if (UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(Actor))
	{
		if (PawnStateSubsystem->CheckAssetValid(Asset))
		{
			return UPawnStateLibrary::CanEnterPawnState(Actor, Asset->PawnState.PawnStateTag);
		}
	}
	return false;
}

bool UPawnStateLibrary::CanEnterPawnState(AActor* Actor, const FGameplayTag& PawnStateTag)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		FString ErrMsg;
		return Component->CanEnterPawnState(PawnStateTag, ErrMsg);
	}
	return false;
}

int32 UPawnStateLibrary::EnterPawnState(AActor* Actor, const FGameplayTag& PawnStateTag, UObject* SourceObject, UObject* Instigator)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->InternalEnterPawnState(PawnStateTag, SourceObject, Instigator);
	}
	else
	{
		PAWNSTATE_LOG(Error, TEXT("%s[%s], cannot get pawn state component for actor[%s]"), *FString(__FUNCTION__), *PawnStateTag.ToString(), *GetNameSafe(Actor));
	}
	return 0;
}

bool UPawnStateLibrary::LeavePawnState(AActor* Actor, int32 InstanceID, UObject* Instigator)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->LeavePawnState(InstanceID, Instigator);
	}
	else
	{
		PAWNSTATE_LOG(Error, TEXT("%s, cannot get pawn state component for actor[%s]"), *FString(__FUNCTION__), *GetNameSafe(Actor));
	}
	return false;
}

UPawnStateEvent* UPawnStateLibrary::GetEnterEventByTag(AActor* Actor, FGameplayTag PawnStateTag)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->GetEnterEventByTag(PawnStateTag);
	}
	else
	{
		PAWNSTATE_LOG(Error, TEXT("%s[%s], cannot get pawn state component for actor[%s]"), *FString(__FUNCTION__), *PawnStateTag.ToString(), *GetNameSafe(Actor));
	}
	return nullptr;
}

UPawnStateEvent* UPawnStateLibrary::GetLeaveEventByTag(AActor* Actor, FGameplayTag PawnStateTag)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->GetLeaveEventByTag(PawnStateTag);
	}
	else
	{
		PAWNSTATE_LOG(Error, TEXT("%s[%s], cannot get pawn state component for actor[%s]"), *FString(__FUNCTION__), *PawnStateTag.ToString(), *GetNameSafe(Actor));
	}
	return nullptr;
}
