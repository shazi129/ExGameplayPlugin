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
		EPawnStateRelation Relation = PawnStateSubsystem->GetStateRelation(PawnState, NewPawnState);
		if (Relation == EPawnStateRelation::E_FORBID_ENTER)
		{
			return false;
		}
	}
	return true;
}


bool UPawnStateComponent::EnterState(const FGameplayTag& NewPawnState)
{
	//重复进入
	if (CurrentPawnStates.HasTagExact(NewPawnState))
	{
		return true;
	}

	//是否被其他state限制之类的
	if (!CanEnterState(NewPawnState))
	{
		return false;
	}

	//处理互斥state
	FGameplayTagContainer NeedRemoveStates;
	for (const FGameplayTag& PawnState : CurrentPawnStates)
	{
		EPawnStateRelation Relation = UPawnStateSubsystem::GetPawnStateSubsystem(this)->GetStateRelation(NewPawnState, PawnState);
		if (Relation == EPawnStateRelation::E_MUTEX)
		{
			NeedRemoveStates.AddTag(PawnState);
		}
	}
	//移除state
	for (const FGameplayTag& PawnState : NeedRemoveStates)
	{
		LeaveState(PawnState);
	}

	//添加state
	CurrentPawnStates.AddTag(NewPawnState);
	FPawnStateEvent& Event = GetEnterEvent(NewPawnState);
	if (Event.Delegate.IsBound())
	{
		Event.Delegate.Broadcast(NewPawnState);
	}
	return true;
}


bool UPawnStateComponent::LeaveState(const FGameplayTag& PawnState)
{
	if (CurrentPawnStates.HasTagExact(PawnState))
	{
		CurrentPawnStates.RemoveTag(PawnState);
		FPawnStateEvent& Event = GetLeaveEvent(PawnState);
		if (Event.Delegate.IsBound())
		{
			Event.Delegate.Broadcast(PawnState);
		}
	}
	return true;
}

bool UPawnStateComponent::HasState(const FGameplayTag& PawnState, bool Exactly)
{
	if (Exactly)
	{
		return CurrentPawnStates.HasTagExact(PawnState);
	}
	else
	{
		return CurrentPawnStates.HasTag(PawnState);
	}
}

FPawnStateEvent& UPawnStateComponent::GetEnterEvent(const FGameplayTag& PawnState)
{
	return PawnStateEnterEvent.FindOrAdd(PawnState);
}

FPawnStateEvent& UPawnStateComponent::GetLeaveEvent(const FGameplayTag& PawnState)
{
	return PawnStateLeaveEvent.FindOrAdd(PawnState);
}

FString UPawnStateComponent::ToString()
{
	return CurrentPawnStates.ToString();
}
