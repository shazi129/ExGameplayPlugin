#include "PawnStateLibrary.h"
#include "PawnStateSettingSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "PawnStateModule.h"
#include "ExGameplayLibrary.h"


UPawnStateComponent* UPawnStateLibrary::GetLocalPawnStateComponent(const UObject* WorldContextObject)
{
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0))
	{
		return  UPawnStateLibrary::GetPawnStateComponent(Character);
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

bool UPawnStateLibrary::CanEnterPawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->CanEnterPawnState(PawnStateInstance);
	}
	else
	{
		PAWNSTATE_LOG(Error, TEXT("%s[%s], cannot get pawn state component for actor[%s]"), *FString(__FUNCTION__), *PawnStateInstance.ToString(), *GetNameSafe(Actor));
	}

	return false;
}

bool UPawnStateLibrary::EnterPawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->EnterPawnState(PawnStateInstance);
	}
	else
	{
		PAWNSTATE_LOG(Error, TEXT("%s[%s], cannot get pawn state component for actor[%s]"), *FString(__FUNCTION__), *PawnStateInstance.ToString(), *GetNameSafe(Actor));
	}
	return false;
}

bool UPawnStateLibrary::LeavePawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->LeavePawnState(PawnStateInstance);
	}
	else
	{
		PAWNSTATE_LOG(Error, TEXT("%s[%s], cannot get pawn state component for actor[%s]"), *FString(__FUNCTION__), *PawnStateInstance.ToString(), *GetNameSafe(Actor));
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

FPawnStateInstance UPawnStateLibrary::GetGlobalPawnStateInstance(FName PawnStateTagName, UObject* SourceObject)
{
	UPawnStateSettingSubsystem* SubSystem = UPawnStateSettingSubsystem::GetSubsystem(SourceObject);
	if (SubSystem == nullptr)
	{
		PAWNSTATE_LOG(Error, TEXT("%s error, cannot get subsystem by object[%s]"), *FString(__FUNCTION__), *GetNameSafe(SourceObject));
		return FPawnStateInstance();
	}

	FGameplayTag PawnStateTag = UExGameplayLibrary::RequestGameplayTag(PawnStateTagName);
	if (const UPawnStateAsset* Asset = SubSystem->GetGlobalPawnStateAsset(PawnStateTag))
	{
		return FPawnStateInstance(Asset, SourceObject);
	}
	return FPawnStateInstance();
}
