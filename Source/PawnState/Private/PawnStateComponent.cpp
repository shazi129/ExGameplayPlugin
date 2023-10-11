#include "PawnStateComponent.h"
#include "PawnStateSettingSubsystem.h"
#include "ExGameplayLibrary.h"
#include "PawnStateModule.h"
#include "PawnStateSubsystem.h"
#include "Net/UnrealNetwork.h"

std::atomic<int32> UPawnStateComponent::InstanceIDGenerator = 0;

void UPawnStateComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);

	//加入World的PawnState
	UWorld* World = this->GetWorld();
	if (!World)
	{
		return;
	}
	FString WorldPackageFullName = UExGameplayLibrary::GetPackageFullName(World);
	for (auto& WorldPawnStateInfo : GetDefault<UPawnStateSettings>()->WorldPawnStates)
	{
		if (!WorldPawnStateInfo.MainWorld.IsNull() && WorldPackageFullName == WorldPawnStateInfo.MainWorld.GetLongPackageName())
		{
			InternalEnterPawnState(WorldPawnStateInfo.WorldPawnState, World, nullptr);
			break;
		}
	}
}

void UPawnStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	PawnStateEnterEvent.Empty();
	PawnStateLeaveEvent.Empty();
}

void UPawnStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UPawnStateComponent, CurrentPawnStateTags, COND_SimulatedOnly)
}

FString UPawnStateComponent::ToString()
{
	return CurrentPawnStateTags.ToString();
}

bool UPawnStateComponent::CanEnterPawnState(const FGameplayTag& NewPawnStateTag, FString& ErrMsg)
{
	if (!NewPawnStateTag.IsValid())
	{
		ErrMsg = FString::Printf(TEXT("Invalidate PawnStateTag: %s"), *NewPawnStateTag.ToString());
		return false;
	}

	UPawnStateSubsystem* PawnSateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	return PawnSateSubsystem->CanEnterPawnState(NewPawnStateTag, CurrentPawnStateTags, ErrMsg);
}

void UPawnStateComponent::RebuildCurrentTag()
{
	CurrentPawnStateTags.Reset();
	for (const FPawnStateInstance& Instance : PawnStateInstances)
	{
		CurrentPawnStateTags.AddTag(Instance.PawnStateTag);
	}
}

int UPawnStateComponent::InternalEnterPawnState(const FGameplayTag& NewPawnStateTag, UObject* SourceObject, UObject* Instigator)
{
	SourceObject = SourceObject != nullptr ? SourceObject : this;

	//如果已经存在，只需要增加基数
	FPawnStateInstance* SameInstance = FindPawnStateInstance(NewPawnStateTag, SourceObject, Instigator);
	if (SameInstance)
	{
		SameInstance->IDList.Add(++InstanceIDGenerator);
		PAWNSTATE_LOG(Log, TEXT("%s: %s, Role:%d"), *FString(__FUNCTION__), *SameInstance->ToString(), GetOwner()->GetLocalRole());

		return InstanceIDGenerator;
	}

	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);

	//使互斥的PawnState退出
	TArray<FGameplayTag> RemovedStates;
	for (FPawnStateInstance& Instance : PawnStateInstances)
	{
		EPawnStateRelation Relation = PawnStateSubsystem->GetRelation(Instance.PawnStateTag, NewPawnStateTag);
		if (Relation == EPawnStateRelation::E_Mutex)
		{
			RemovedStates.Add(Instance.PawnStateTag);
		}
	}

	//互斥的发起者
	UObject* MutexInstigator = Instigator ? Instigator : SourceObject;
	for (const FGameplayTag& State : RemovedStates)
	{
		LeaveAllPawnStateTag(State, MutexInstigator);
	}

	//加入新的PawnState
	PawnStateInstances.Emplace();
	FPawnStateInstance& NewInstance = PawnStateInstances.Last();
	NewInstance.PawnStateTag = NewPawnStateTag;
	NewInstance.SourceObject = SourceObject;
	NewInstance.Instigator = Instigator;
	NewInstance.IDList.Add(++InstanceIDGenerator);
	PAWNSTATE_LOG(Log, TEXT("%s: %s, Role:%d"), *FString(__FUNCTION__), *NewInstance.ToString(), GetOwner()->GetLocalRole());

	RebuildCurrentTag();

	UPawnStateEvent* Event = GetEnterEventByTag(NewPawnStateTag);
	if (Event && Event->Delegate.IsBound())
	{
		Event->Delegate.Broadcast(NewInstance);
	}

	HandleStateEvent(EPawnStateEventTriggerType::E_Enter, NewInstance);

	return InstanceIDGenerator;
}

int UPawnStateComponent::TryEnterPawnState(FGameplayTag PawnStateTag, UObject* SourceObject, UObject* Instigator)
{
	FString ErrorMsg;
	if (!CanEnterPawnState(PawnStateTag, ErrorMsg))
	{
		PAWNSTATE_LOG(Error, TEXT("%s cannot enter %s: %s"), *FString(__FUNCTION__), *PawnStateTag.ToString(), *ErrorMsg);
		return 0;
	}
	return InternalEnterPawnState(PawnStateTag, SourceObject, Instigator);
}

bool UPawnStateComponent::TryEnterPawnStateByAssets(const TArray<UPawnStateAsset*>& Assets, UPARAM(ref) TArray<int32>& Handles, UObject* SourceObject, UObject* Instigator)
{
	static TArray<FGameplayTag> AssetTags;
	AssetTags.Reset();

	for (auto& PawnStateAsset : Assets)
	{
		if (!PawnStateAsset || !PawnStateAsset->StateTag.IsValid())
		{
			return false;
		}

		AssetTags.Add(PawnStateAsset->StateTag);
	}
	return TryEnterPawnStateByTags(AssetTags, Handles, SourceObject, Instigator);
}

bool UPawnStateComponent::TryEnterPawnStateByTags(const TArray<FGameplayTag>& PawnStateTags, UPARAM(ref)TArray<int32>& Handles, UObject* SourceObject, UObject* Instigator)
{
	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	FString ErrorMsg;
	Handles.Reset();

	for (auto& PawnStateTag : PawnStateTags)
	{
		if (!CanEnterPawnState(PawnStateTag, ErrorMsg))
		{
			PAWNSTATE_LOG(Error, TEXT("%s cannot enter %s: %s"), *FString(__FUNCTION__), *PawnStateTag.ToString(), *ErrorMsg);
			return false;
		}
	}

	for (auto& PawnStateTag : PawnStateTags)
	{
		int Handle = InternalEnterPawnState(PawnStateTag, SourceObject, Instigator);
		Handles.Add(Handle);
	}

	return true;
}

bool UPawnStateComponent::LeavePawnState(int InstID, UObject* Instigator)
{
	return InternalLeavePawnState(InstID, Instigator);
}

bool UPawnStateComponent::LeaveAllPawnStateTag(FGameplayTag PawnStateTag, UObject* Instigator)
{
	TArray<int32> RemovedInstanceID;
	for (auto& PawnStateInst : PawnStateInstances)
	{
		if (PawnStateTag == PawnStateInst.PawnStateTag)
		{
			RemovedInstanceID.Append(PawnStateInst.IDList);
		}
	}
	for (int32 InstanceID : RemovedInstanceID)
	{
		InternalLeavePawnState(InstanceID, Instigator);
	}
	return false;
}

bool UPawnStateComponent::InternalLeavePawnState(int InstanceID, UObject* Instigator)
{
	//先移除再回调，防止回调中还有移除操作
	FPawnStateInstance InstancdRemove;

	for (int i = PawnStateInstances.Num() - 1; i >= 0; i--)
	{
		int Index = PawnStateInstances[i].IDList.Find(InstanceID);
		if (Index >= 0)
		{
			PawnStateInstances[i].IDList.RemoveAt(Index);
			if (PawnStateInstances[i].IDList.IsEmpty())
			{
				InstancdRemove = PawnStateInstances[i];
				PawnStateInstances.RemoveAt(i);
			}
			else
			{
				PAWNSTATE_LOG(Log, TEXT("%s: %s, Role:%d"), *FString(__FUNCTION__), *PawnStateInstances[i].ToString(), GetOwner()->GetLocalRole());
			}
			break;
		}
	}

	if (InstancdRemove.IsValid())
	{
		InstancdRemove.Instigator =  Instigator;
		PAWNSTATE_LOG(Log, TEXT("%s: %s, Role:%d"), *FString(__FUNCTION__), *InstancdRemove.ToString(), GetOwner()->GetLocalRole());

		UPawnStateEvent* Event = GetLeaveEventByTag(InstancdRemove.PawnStateTag);
		if (Event && Event->Delegate.IsBound())
		{
			Event->Delegate.Broadcast(InstancdRemove);
		}
		HandleStateEvent(EPawnStateEventTriggerType::E_Leave, InstancdRemove);
		RebuildCurrentTag();
	}
	return true;
}

void UPawnStateComponent::HandleStateEvent(EPawnStateEventTriggerType TriggerType, const FPawnStateInstance& Instance)
{
	//相关事件，只针对定义了PawnState的tag
	const UPawnStateAsset* PawnStateAsset = UPawnStateSubsystem::GetSubsystem(this)->GetPawnStateAsset(Instance.PawnStateTag);
	if (!PawnStateAsset)
	{
		return;
	}

	for (auto& InstancedStruct : PawnStateAsset->EventList)
	{
		const UScriptStruct* StructType = InstancedStruct.GetScriptStruct();
		if (StructType->IsChildOf(FPawnStateEventItem::StaticStruct()))
		{
			FPawnStateEventItem& EventItem = InstancedStruct.GetMutable<FPawnStateEventItem>();
			EventItem.Execute(TriggerType, Instance, this);
		}
	}
}

bool UPawnStateComponent::HasPawnStateTag(FGameplayTag PawnStateTag)
{
	for (const FPawnStateInstance& Instance : PawnStateInstances)
	{
		if (Instance.PawnStateTag == PawnStateTag)
		{
			return true;
		}
	}
	return false;
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
		PawnStateLeaveEvent.Add(PawnStateTag, Event);
	}
	return PawnStateLeaveEvent[PawnStateTag];
}

const TArray<FPawnStateInstance>& UPawnStateComponent::GetPawnStateInstances()
{
	return PawnStateInstances;
}

FPawnStateInstance* UPawnStateComponent::FindPawnStateInstance(const FGameplayTag& PawnStateTag, UObject* SourceObject, UObject* Instigator)
{
	for (FPawnStateInstance& Instance : PawnStateInstances)
	{
		if (Instance.PawnStateTag == PawnStateTag && Instance.SourceObject == SourceObject && Instance.Instigator == Instigator)
		{
			return &Instance;
		}
	}
	return nullptr;
}

bool UPawnStateComponent::SendMsgToServer_Validate(const FGameplayTag& MsgTag, const FInstancedStruct& MsgBody)
{
	return true;
}

void UPawnStateComponent::SendMsgToServer_Implementation(const FGameplayTag& MsgTag, const FInstancedStruct& MsgBody)
{
	UPawnStateSubsystem* PawnStateSystem = UPawnStateSubsystem::GetSubsystem(this);
	PawnStateSystem->HandleServerMsg(this, MsgTag, MsgBody);
}

bool UPawnStateComponent::SendMsgToClient_Validate(const FGameplayTag& MsgTag, const FInstancedStruct& MsgBody)
{
	return true;
}

void UPawnStateComponent::SendMsgToClient_Implementation(const FGameplayTag& MsgTag, const FInstancedStruct& MsgBody)
{
	UPawnStateSubsystem* PawnStateSystem = UPawnStateSubsystem::GetSubsystem(this);
	PawnStateSystem->HandleClientMsg(this, MsgTag, MsgBody);
}

