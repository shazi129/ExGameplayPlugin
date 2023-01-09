#include "PawnState/PawnStateLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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

bool UPawnStateLibrary::CanEnterPawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->CanEnterPawnState(PawnStateInstance);
	}
	return false;
}

bool UPawnStateLibrary::EnterPawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->EnterPawnState(PawnStateInstance);
	}
	return false;
}

bool UPawnStateLibrary::LeavePawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance)
{
	if (UPawnStateComponent* Component = UPawnStateLibrary::GetPawnStateComponent(Actor))
	{
		return Component->LeavePawnState(PawnStateInstance);
	}
	return false;
}
