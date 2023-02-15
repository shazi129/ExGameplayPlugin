#include "PawnStateComponent.h"
#include "PawnStateSettingSubsystem.h"
#include "ExGameplayLibrary.h"
#include "PawnStateModule.h"

void UPawnStateComponent::BeginPlay()
{
	Super::BeginPlay();

	//加入World的PawnState
	UWorld* World = this->GetWorld();
	if (World && UExGameplayLibrary::IsClient(this))
	{
		if (const FWorldPawnStateInfo* WorldPawnStateInfo = UPawnStateSettingSubsystem::GetSubsystem(this)->GetWorldStateInfo(this->GetWorld()))
		{
			if (WorldPawnStateInfo->WorldState != nullptr)
			{
				const FGameplayTag& PawnStateTag = WorldPawnStateInfo->WorldState->PawnState.PawnStateTag;
				if (PawnStateTag.IsValid())
				{
					EnterPawnState(FPawnStateInstance(WorldPawnStateInfo->WorldState, World, nullptr));
				}
			}
		}
	}
}

void UPawnStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (auto& EnterEventItem : PawnStateEnterEvent)
	{
		EnterEventItem.Value->RemoveFromRoot();
	}
	PawnStateEnterEvent.Empty();

	for (auto& LeaveEventItem : PawnStateLeaveEvent)
	{
		LeaveEventItem.Value->RemoveFromRoot();
	}
	PawnStateLeaveEvent.Empty();
}

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
	if (CurrentPawnStateTags.HasAnyExact(PawnStateInstance.PawnState.ActivateBlockedTags))
	{
		if (ErrMsg)
		{
			*ErrMsg = FString::Printf(TEXT("%s is blocked by current state"), *PawnStateInstance.ToString());
		}
		return false;
	}

	//检查require
	if (!CurrentPawnStateTags.HasAllExact(PawnStateInstance.PawnState.RequiredTags))
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
		CurrentPawnStateTags.AddTag(Instance.PawnState.PawnStateTag);
	}
}

bool UPawnStateComponent::EnterPawnState(const FPawnStateInstance& NewPawnStateInstance)
{
	if (!NewPawnStateInstance.IsValid())
	{
		PAWNSTATE_LOG(Error, TEXT("%s Error: Paramter error"), *FString(__FUNCTION__));
		return false;
	}

	if (HasPawnState(NewPawnStateInstance))
	{
		PAWNSTATE_LOG(Warning, TEXT("%s warning: enter a exist state %s"), *FString(__FUNCTION__), *NewPawnStateInstance.ToString());
		return true;
	}

	FString ErrMsg;
	if (!InternalCanEnterPawnState(NewPawnStateInstance, &ErrMsg))
	{
		PAWNSTATE_LOG(Error, TEXT("%s Error: %s"), *FString(__FUNCTION__), *ErrMsg);
		return false;
	}

	PAWNSTATE_LOG(Log, TEXT("%s: %s"), *FString(__FUNCTION__), *NewPawnStateInstance.ToString());

	//使互斥的PawnState退出
	TArray<FPawnStateInstance*> RemovedInstance;
	for (FPawnStateInstance& Instance : PawnStateInstances)
	{
		if (Instance.PawnState.CancelledTags.HasTagExact(NewPawnStateInstance.PawnState.PawnStateTag))
		{
			RemovedInstance.Add(&Instance);
		}
		else if (NewPawnStateInstance.PawnState.CancelOtherTags.HasTagExact(Instance.PawnState.PawnStateTag))
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

	UPawnStateEvent* Event = GetEnterEventByTag(NewPawnStateInstance.PawnState.PawnStateTag);
	if (Event && Event->Delegate.IsBound())
	{
		Event->Delegate.Broadcast(NewPawnStateInstance);
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
			PAWNSTATE_LOG(Log, TEXT("%s: %s"), *FString(__FUNCTION__), *PawnStateInstances[i].ToString());

			FPawnStateInstance Instance(PawnStateInstances[i]);
			Instance.Instigator = Instigator;

			PawnStateInstances.RemoveAt(i);
			RebuildCurrentTag();

			UPawnStateEvent* Event = GetLeaveEventByTag(PawnStateInstance.PawnState.PawnStateTag);
			if (Event && Event->Delegate.IsBound())
			{
				Event->Delegate.Broadcast(Instance);
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

UPawnStateEvent* UPawnStateComponent::GetEnterEvent(const UPawnStateAsset* PawnStateAsset)
{
	if (!PawnStateAsset)
	{
		PAWNSTATE_LOG(Error, TEXT("%s Error: PawnState is null"), *FString(__FUNCTION__));
		return nullptr;
	}
	return GetEnterEventByTag(PawnStateAsset->PawnState.PawnStateTag);
	
}

UPawnStateEvent* UPawnStateComponent::GetLeaveEvent(const UPawnStateAsset* PawnStateAsset)
{
	if (!PawnStateAsset)
	{
		PAWNSTATE_LOG(Error, TEXT("%s Error: PawnState is null"), *FString(__FUNCTION__));
		return nullptr;
	}
	return GetLeaveEventByTag(PawnStateAsset->PawnState.PawnStateTag);
}

UPawnStateEvent* UPawnStateComponent::GetEnterEventByTag(FGameplayTag PawnStateTag)
{
	if (!PawnStateTag.IsValid())
	{
		PAWNSTATE_LOG(Error, TEXT("%s Error: Invalid PawnStateTag[%s]"), *FString(__FUNCTION__), *PawnStateTag.ToString());
		return nullptr;
	}

	if (!PawnStateEnterEvent.Contains(PawnStateTag))
	{
		UPawnStateEvent* Event = NewObject<UPawnStateEvent>(this);
		Event->AddToRoot();
		PawnStateEnterEvent.Add(PawnStateTag, Event);
	}
	return PawnStateEnterEvent[PawnStateTag];
}

UPawnStateEvent* UPawnStateComponent::GetLeaveEventByTag(FGameplayTag PawnStateTag)
{
	if (!PawnStateTag.IsValid())
	{
		PAWNSTATE_LOG(Error, TEXT("%s Error: Invalid PawnStateTag[%s]"), *FString(__FUNCTION__), *PawnStateTag.ToString());
		return nullptr;
	}
	if (!PawnStateLeaveEvent.Contains(PawnStateTag))
	{
		UPawnStateEvent* Event = NewObject<UPawnStateEvent>(this);
		Event->AddToRoot();
		PawnStateLeaveEvent.Add(PawnStateTag, Event);
	}
	return PawnStateLeaveEvent[PawnStateTag];
}

const TArray<FPawnStateInstance>& UPawnStateComponent::GetPawnStateInstances()
{
	return PawnStateInstances;
}
