#include "PawnState/PawnStateComponent.h"
#include "PawnState/PawnStateSubsystem.h"

bool UPawnStateComponent::CanEnterState(const FGameplayTag& NewPawnState)
{
	if (!NewPawnState.IsValid())
	{
		return false;
	}

	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetPawnStateSubsystem(this);
	for (const FGameplayTag& PawnState : CurrentPawnStates)
	{
		if (PawnState == NewPawnState)
		{
			return true;
		}
		EPawnStateRelation Relation = PawnStateSubsystem->GetRelation(PawnState, NewPawnState);
		if (Relation == EPawnStateRelation::E_FORBID_ENTER)
		{
			return false;
		}
	}
	return true;
}


bool UPawnStateComponent::EnterState(const FGameplayTag& NewPawnState)
{
	if (!CanEnterState(NewPawnState))
	{
		return false;
	}

	CurrentPawnStates.AddTag(NewPawnState);
	return true;
}


bool UPawnStateComponent::LeaveState(const FGameplayTag& PawnState)
{
	CurrentPawnStates.RemoveTag(PawnState);
	return true;
}