#include "PawnStateComponent.h"
#include "ExGameplayLibrary.h"
#include "PawnStateModule.h"
#include "PawnStateSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "DisplayDebugHelpers.h"
#include "GameFramework/HUD.h"
#include "PawnStateLibrary.h"
#include "Engine/Canvas.h"

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
	DOREPLIFETIME_CONDITION(UPawnStateComponent, ReplicatePawnStateTags, COND_None)
}

void UPawnStateComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
	if (DisplayInfo.IsDisplayOn(TEXT("PawnState")))
	{
		UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(HUD->GetCurrentDebugTargetActor());
		if (PawnStateComponent)
		{
			PawnStateComponent->CollectDebugContent();
			PawnStateComponent->DebugDisplayProxy.DisplayDebug(HUD, Canvas, DisplayInfo, YL, YPos);
		}
	}
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
	if (PawnSateSubsystem)
	{
		FGameplayTagContainer PawnStateTags(CurrentPawnStateTags);
		PawnStateTags.AppendTags(ReplicatePawnStateTags);
		return PawnSateSubsystem->CanEnterPawnState(NewPawnStateTag, PawnStateTags, ErrMsg);
	}

	return false;
}

bool UPawnStateComponent::CanEnterPawnStates(const FGameplayTagContainer& PawnStates, FString& ErrMsg)
{
	for (const FGameplayTag& Tag : PawnStates)
	{
		if (!CanEnterPawnState(Tag, ErrMsg))
		{
			return false;
		}
	}
	return true;
}

void UPawnStateComponent::OnRep_PawnStateTags(const FGameplayTagContainer& OldValue)
{
	//只在模拟端处理State事件
	ENetRole LocalRole = GetOwner()->GetLocalRole();
	if (LocalRole == ENetRole::ROLE_SimulatedProxy)
	{
		FPawnStateInstance Instance;

		//找出被删的
		for (const FGameplayTag& OldTag : OldValue)
		{
			if (!OldTag.MatchesAnyExact(ReplicatePawnStateTags))
			{
				Instance.PawnStateTag = OldTag;
				HandleStateEvent(EPawnStateEventType::E_Leave, Instance);
			}
		}

		//找出新增的
		for (const FGameplayTag& NewTag : ReplicatePawnStateTags)
		{
			if (!NewTag.MatchesAnyExact(OldValue))
			{
				Instance.PawnStateTag = NewTag;
				HandleStateEvent(EPawnStateEventType::E_Enter, Instance);
			}
		}
	}
}

void UPawnStateComponent::CollectDebugContent()
{
	DebugDisplayProxy.ClearContent();
	DebugDisplayProxy.AddDebugLine(TEXT("PawnState Info"), FColor::Red, GEngine->GetMediumFont());

	DebugDisplayProxy.AddDebugLine(FString::Printf(TEXT("===Local State Num:%d"), PawnStateInstances.Num()), FColor::Green, GEngine->GetMediumFont());
	for (const auto& Instance : PawnStateInstances)
	{
		DebugDisplayProxy.AddDebugLine(Instance.ToString(), FColor::Yellow);
	}
	DebugDisplayProxy.AddDebugLine(FString::Printf(TEXT("===Server State Num:%d"), ReplicatePawnStateTags.Num()), FColor::Green, GEngine->GetMediumFont());
	for (const auto& Tag : ReplicatePawnStateTags)
	{
		DebugDisplayProxy.AddDebugLine(Tag.ToString(), FColor::Yellow);
	}
}

void UPawnStateComponent::RebuildCurrentTag()
{
	CurrentPawnStateTags.Reset();
	for (const FPawnStateInstance& Instance : PawnStateInstances)
	{
		CurrentPawnStateTags.AddTag(Instance.PawnStateTag);
	}

	//如果是主控端，准备复制
	ENetRole LocalRole = GetOwner()->GetLocalRole();
	if (LocalRole == ENetRole::ROLE_Authority)
	{
		ReplicatePawnStateTags.Reset();
		ReplicatePawnStateTags.AppendTags(CurrentPawnStateTags);
	}
}

int UPawnStateComponent::InternalEnterPawnState(const FGameplayTag& NewPawnStateTag, UObject* SourceObject, UObject* Instigator)
{
	//只在主控端和服务端运行
	ENetRole Role = GetOwner()->GetLocalRole();
	if (Role != ENetRole::ROLE_Authority && Role != ENetRole::ROLE_AutonomousProxy)
	{
		PAWNSTATE_LOG(Log, TEXT("%s should execute in Authority or AutonomousProxy"), *FString(__FUNCTION__));
		return -1;
	}

	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	if (!PawnStateSubsystem)
	{
		PAWNSTATE_LOG(Error, TEXT("%s: cannot get pawnstate subsystem"), *FString(__FUNCTION__));
		return -1;
	}

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

	SourceObject = SourceObject != nullptr ? SourceObject : this;

	//如果已经存在，只需要增加基数
	FPawnStateInstance* SameInstance = FindPawnStateInstance(NewPawnStateTag, SourceObject, Instigator);
	if (SameInstance)
	{
		SameInstance->IDList.Add(++InstanceIDGenerator);
		PAWNSTATE_LOG(Log, TEXT("%s: %s, Role:%d"), *FString(__FUNCTION__), *SameInstance->ToString(), GetOwner()->GetLocalRole());

		return InstanceIDGenerator;
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

	HandleStateEvent(EPawnStateEventType::E_Enter, NewInstance);

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
	//只在主控端和服务端运行
	ENetRole Role = GetOwner()->GetLocalRole();
	if (Role != ENetRole::ROLE_Authority && Role != ENetRole::ROLE_AutonomousProxy)
	{
		PAWNSTATE_LOG(Log, TEXT("%s: enter cannot run on %d"), *FString(__FUNCTION__), Role);
		return false;
	}

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
		HandleStateEvent(EPawnStateEventType::E_Leave, InstancdRemove);
		RebuildCurrentTag();
	}
	return true;
}

void UPawnStateComponent::HandleStateEvent(EPawnStateEventType TriggerType, const FPawnStateInstance& Instance)
{
	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	if (!PawnStateSubsystem)
	{
		return;
	}

	if (PawnStateEventDelegate.IsBound())
	{
		PawnStateEventDelegate.Broadcast(TriggerType, Instance);
	}

	//通知
	UPawnStateEventWrapper* EventWrapper = GetEventByTag(Instance.PawnStateTag);
	if (EventWrapper)
	{
		EventWrapper->Delegate.Broadcast(TriggerType, Instance);
	}

	UPawnStateEvent* Event = nullptr;
	if (TriggerType == EPawnStateEventType::E_Enter)
	{
		Event = GetEnterEventByTag(Instance.PawnStateTag);
	}
	else if (TriggerType == EPawnStateEventType::E_Leave)
	{
		Event = GetLeaveEventByTag(Instance.PawnStateTag);
	}
	if (Event && Event->Delegate.IsBound())
	{
		Event->Delegate.Broadcast(Instance);
	}

	TArray<FInstancedStruct>* EventStructs = PawnStateInstancedEventMap.Find(Instance.PawnStateTag);
	if (EventStructs == nullptr)
	{
		if (const UPawnStateAsset* PawnStateAsset = PawnStateSubsystem->GetPawnStateAsset(Instance.PawnStateTag))
		{
			EventStructs = &PawnStateInstancedEventMap.Add(Instance.PawnStateTag, PawnStateAsset->EventList);
		}
	}

	if (EventStructs != nullptr)
	{
		for (auto& InstancedStruct : *EventStructs)
		{
			const UScriptStruct* StructType = InstancedStruct.GetScriptStruct();
			if (StructType->IsChildOf(FPawnStateEventItem::StaticStruct()))
			{
				FPawnStateEventItem& EventItem = InstancedStruct.GetMutable<FPawnStateEventItem>();
				EventItem.Execute(TriggerType, Instance, this);
			}
		}
	}
	
}

bool UPawnStateComponent::HasPawnStateTag(FGameplayTag PawnStateTag, bool OnlyLocal)
{
	//考虑本地的
	for (const FPawnStateInstance& Instance : PawnStateInstances)
	{
		if (Instance.PawnStateTag == PawnStateTag)
		{
			return true;
		}
	}

	//考虑复制过来的
	if (!OnlyLocal)
	{
		ENetRole LocalRole = GetOwner()->GetLocalRole();
		if (LocalRole != ENetRole::ROLE_Authority)
		{
			return ReplicatePawnStateTags.HasTagExact(PawnStateTag);
		}
	}

	return false;
}

bool UPawnStateComponent::HasPawnStateAsset(TSoftObjectPtr<UPawnStateAsset>& PawnState)
{
	if (PawnState.IsNull())
	{
		return false;
	}
	if (UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this))
	{
		UPawnStateAsset* PawnStateAsset = PawnState.LoadSynchronous();
		if (PawnStateSubsystem->LoadPawnStateAsset(PawnStateAsset))
		{
			if (HasPawnStateTag(PawnStateAsset->StateTag))
			{
				return true;
			}
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

UPawnStateEventWrapper* UPawnStateComponent::GetEventByTag(FGameplayTag PawnStateTag)
{
	if (!PawnStateTag.IsValid())
	{
		PAWNSTATE_LOG(Error, TEXT("%s Error: Invalid PawnStateTag[%s]"), *FString(__FUNCTION__), *PawnStateTag.ToString());
		return nullptr;
	}
	if (!PawnStateEventMap.Contains(PawnStateTag))
	{
		UPawnStateEventWrapper* EventWrappper = NewObject<UPawnStateEventWrapper>(this);
		PawnStateEventMap.Add(PawnStateTag, EventWrappper);
	}
	return PawnStateEventMap[PawnStateTag];
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

