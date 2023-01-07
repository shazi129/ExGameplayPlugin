#include "PawnState/PawnStateComponent.h"
#include "ExGameplayPluginModule.h"

FString UPawnStateComponent::ToString()
{
	return CurrentPawnStateTags.ToString();
}

bool UPawnStateComponent::CanEnterPawnState(const FPawnStateInstance& PawnStateInstance)
{
	return InternalCanEnterPawnState(PawnStateInstance, nullptr);
}

bool UPawnStateComponent::InternalCanEnterPawnState(const FPawnStateInstance& PawnStateInstance, FString* ErrMsg)
{
	if (!PawnStateInstance.IsValid())
	{
		if (ErrMsg)
		{
			*ErrMsg = FString::Printf(TEXT("Invalidate PawnStateInstance"));
		}
		return false;
	}

	//看有没有block的
	if (CurrentPawnStateTags.HasAnyExact(PawnStateInstance.PawnState->ActivateBlockedTags))
	{
		if (ErrMsg)
		{
			*ErrMsg = FString::Printf(TEXT("%s is blocked by current state"), *PawnStateInstance.ToString());
		}
		return false;
	}

	//检查require
	if (!CurrentPawnStateTags.HasAllExact(PawnStateInstance.PawnState->RequiredTags))
	{
		if (ErrMsg)
		{
			*ErrMsg = FString::Printf(TEXT("%s miss required state"), *PawnStateInstance.ToString());
		}
		return false;
	}

	return true;
}

void UPawnStateComponent::RebuildCurrentTag()
{
	CurrentPawnStateTags.Reset();
	for (const FPawnStateInstance& Instance : PawnStateInstances)
	{
		CurrentPawnStateTags.AddTag(Instance.PawnState->PawnStateTag);
	}
}

bool UPawnStateComponent::EnterPawnState(const FPawnStateInstance& NewPawnStateInstance)
{
	if (!NewPawnStateInstance.IsValid())
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s Error: Paramter error"), *FString(__FUNCTION__));
		return false;
	}

	if (HasPawnState(NewPawnStateInstance))
	{
		EXGAMEPLAY_LOG(Warning, TEXT("%s warning: enter a exist state %s"), *FString(__FUNCTION__), *NewPawnStateInstance.ToString());
		return true;
	}

	FString ErrMsg;
	if (!InternalCanEnterPawnState(NewPawnStateInstance, &ErrMsg))
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s Error: %s"), *FString(__FUNCTION__), *ErrMsg);
		return false;
	}

	EXGAMEPLAY_LOG(Log, TEXT("%s: %s"), *FString(__FUNCTION__), *NewPawnStateInstance.ToString());

	//使互斥的PawnState退出
	TArray<FPawnStateInstance*> RemovedInstance;
	for (FPawnStateInstance& Instance : PawnStateInstances)
	{
		if (Instance.PawnState->CancelledTags.HasTagExact(NewPawnStateInstance.PawnState->PawnStateTag))
		{
			RemovedInstance.Add(&Instance);
		}
		else if (NewPawnStateInstance.PawnState->CancelOtherTags.HasTagExact(Instance.PawnState->PawnStateTag))
		{
			RemovedInstance.Add(&Instance);
		}
	}

	UObject* Instigator = NewPawnStateInstance.Instigator ? NewPawnStateInstance.Instigator : NewPawnStateInstance.SourceObject;
	for (FPawnStateInstance* Instance : RemovedInstance)
	{
		InternalLeavePawnState(*Instance, Instigator);
	}

	//加入新的PawnState
	PawnStateInstances.Add(NewPawnStateInstance);
	RebuildCurrentTag();

	FPawnStateEvent& Event = GetEnterEvent(NewPawnStateInstance.PawnState);
	if (Event.Delegate.IsBound())
	{
		Event.Delegate.Broadcast(NewPawnStateInstance);
	}

	return true;
}

bool UPawnStateComponent::LeavePawnState(const FPawnStateInstance& PawnStateInstance)
{
	return InternalLeavePawnState(PawnStateInstance);
}

bool UPawnStateComponent::InternalLeavePawnState(const FPawnStateInstance& PawnStateInstance, UObject* Instigator)
{
	for (int i = PawnStateInstances.Num() - 1; i >= 0; i--)
	{
		if (PawnStateInstances[i] == PawnStateInstance)
		{
			FPawnStateInstance Instance(PawnStateInstances[i]);
			Instance.Instigator = Instigator;

			PawnStateInstances.RemoveAt(i);
			RebuildCurrentTag();

			FPawnStateEvent& Event = GetEnterEvent(PawnStateInstance.PawnState);
			if (Event.Delegate.IsBound())
			{
				Event.Delegate.Broadcast(Instance);
			}

			break;
		}
	}
	return true;
}

bool UPawnStateComponent::HasPawnState(const FPawnStateInstance& PawnStateInstance)
{
	for (const FPawnStateInstance& Instance : PawnStateInstances)
	{
		if (Instance == PawnStateInstance)
		{
			return true;
		}
	}
	return false;
}

FPawnStateEvent& UPawnStateComponent::GetEnterEvent(const UPawnState* PawnState)
{
	return PawnStateEnterEvent.FindOrAdd(PawnState);
}

FPawnStateEvent& UPawnStateComponent::GetLeaveEvent(const UPawnState* PawnState)
{
	return PawnStateLeaveEvent.FindOrAdd(PawnState);
}
