#include "PawnStateComponent.h"
#include "PawnStateSettingSubsystem.h"
#include "ExGameplayLibrary.h"
#include "PawnStateModule.h"
#include "PawnStateSubsystem.h"

std::atomic<int32> UPawnStateComponent::InstanceIDGenerator = 0;

void UPawnStateComponent::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent = Cast<UAbilitySystemComponent>(GetOwner()->GetComponentByClass(UAbilitySystemComponent::StaticClass()));

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
	const FPawnState& NewPawnState = PawnSateSubsystem->GetPawnState(NewPawnStateTag);
	if (!NewPawnState.IsValid())
	{
		ErrMsg = FString::Printf(TEXT("Cannot find pawn state by tag: %s"), *NewPawnStateTag.ToString());
		return false;
	}	

	//检查相互block
	for (auto& ExistPawnStateTag : CurrentPawnStateTags)
	{
		const FPawnState& ExistPawnState = PawnSateSubsystem->GetPawnState(NewPawnStateTag);
		if (!ExistPawnState.IsValid())
		{
			continue;
		}

		if (ExistPawnState.PawnStateTag.MatchesAny(NewPawnState.ActivateBlockedTags))
		{
			ErrMsg = FString::Printf(TEXT("%s was blocked by %s"), *NewPawnStateTag.ToString(), *ExistPawnState.PawnStateTag.ToString());
			return false;
		}
		else if (NewPawnStateTag.MatchesAny(ExistPawnState.BlockOtherTags))
		{
			ErrMsg = FString::Printf(TEXT("%s was blocked by %s"), *NewPawnStateTag.ToString(), *ExistPawnState.PawnStateTag.ToString());
			return false;
		}
	}

	//检查依赖
	if (NewPawnState.RequiredTags.Num() > 0 && !CurrentPawnStateTags.HasAll(NewPawnState.RequiredTags))
	{
		ErrMsg = FString::Printf(TEXT("%s require check failed"), *NewPawnStateTag.ToString());
		return false;
	}
	return true;
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
	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	const FPawnState& GlobalPawnState = PawnStateSubsystem->GetPawnState(NewPawnStateTag);
	if (GlobalPawnState.IsValid())
	{
		return InternalEnterPawnState(GlobalPawnState, SourceObject, Instigator);
	}
	else
	{
		return InternalEnterPawnState(FPawnState(NewPawnStateTag), SourceObject, Instigator);
	}
}

int UPawnStateComponent::InternalEnterPawnState(const FPawnState& PawnState, UObject* SourceObject, UObject* Instigator)
{
	if (!PawnState.IsValid())
	{
		return 0;
	}

	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	const FGameplayTag& PawnStateTag = PawnState.PawnStateTag;
	SourceObject = SourceObject == nullptr ? SourceObject : this;
	PAWNSTATE_LOG(Log, TEXT("%s: %s"), *FString(__FUNCTION__), *PawnStateTag.ToString());

	//使互斥的PawnState退出
	TArray<FPawnStateInstance*> RemovedInstance;
	for (FPawnStateInstance& Instance : PawnStateInstances)
	{
		const FPawnState& ExistPawnState = PawnStateSubsystem->GetPawnState(Instance.PawnStateTag);
		if (!ExistPawnState.IsValid())
		{
			RemovedInstance.Add(&Instance);
		}
		else if (ExistPawnState.CancelledTags.HasTagExact(PawnStateTag))
		{
			RemovedInstance.Add(&Instance);
		}
		else if (PawnState.CancelOtherTags.HasTagExact(ExistPawnState.PawnStateTag))
		{
			RemovedInstance.Add(&Instance);
		}
	}

	//互斥的发起者
	UObject* MutexInstigator = Instigator ? Instigator : SourceObject;
	for (FPawnStateInstance* Instance : RemovedInstance)
	{
		InternalLeavePawnState(*Instance, MutexInstigator);
	}

	//加入新的PawnState
	PawnStateInstances.Emplace();
	FPawnStateInstance& NewInstance = PawnStateInstances.Last();
	NewInstance.PawnStateTag = PawnStateTag;
	NewInstance.SourceObject = SourceObject;
	NewInstance.Instigator = Instigator;
	NewInstance.InstanceID = ++InstanceIDGenerator;

	RebuildCurrentTag();

	//如果有ASC 添加到ASC中
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->UpdateTagMap(PawnStateTag, 1);
	}

	UPawnStateEvent* Event = GetEnterEventByTag(PawnStateTag);
	if (Event && Event->Delegate.IsBound())
	{
		Event->Delegate.Broadcast(NewInstance);
	}

	return NewInstance.InstanceID;
}

bool UPawnStateComponent::TryEnterPawnStateByAssets(const TArray<UPawnStateAsset*>& Assets, UPARAM(ref) TArray<int32>& Handles, UObject* SourceObject, UObject* Instigator)
{
	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	FString ErrorMsg;
	Handles.Reset();

	for (auto& PawnStateAsset : Assets)
	{
		if (!PawnStateSubsystem->CheckAssetValid(PawnStateAsset))
		{
			PAWNSTATE_LOG(Error, TEXT("%s cannot enter %s: Invalid Asset"), *FString(__FUNCTION__), *GetNameSafe(PawnStateAsset));
			return false;
		}

		if (!CanEnterPawnState(PawnStateAsset->PawnState.PawnStateTag, ErrorMsg))
		{
			PAWNSTATE_LOG(Error, TEXT("%s cannot enter %s: %s"), *FString(__FUNCTION__), *GetNameSafe(PawnStateAsset), *ErrorMsg);
			return false;
		}
	}

	for (auto& PawnStateAsset : Assets)
	{
		int Handle = InternalEnterPawnState(PawnStateAsset->PawnState.PawnStateTag, SourceObject, Instigator);
		Handles.Add(Handle);
	}

	return true;
}

bool UPawnStateComponent::LeavePawnState(int InstID, UObject* Instigator)
{
	for (auto& PawnStateInst : PawnStateInstances)
	{
		if (PawnStateInst.InstanceID == InstID)
		{
			return InternalLeavePawnState(PawnStateInst, Instigator);
		}
	}
	return true;
}

bool UPawnStateComponent::LeaveAllPawnStateTag(FGameplayTag PawnStateTag, UObject* Instigator)
{
	TArray<FPawnStateInstance*> RemovedInstance;
	for (auto& PawnStateInst : PawnStateInstances)
	{
		if (PawnStateTag == PawnStateInst.PawnStateTag)
		{
			RemovedInstance.Add(&PawnStateInst);
		}
	}
	for (FPawnStateInstance* Instance : RemovedInstance)
	{
		InternalLeavePawnState(*Instance, Instigator);
	}
	return false;
}

bool UPawnStateComponent::InternalLeavePawnState(const FPawnStateInstance& PawnStateInstance,UObject* Instigator)
{
	for (int i = PawnStateInstances.Num() - 1; i >= 0; i--)
	{
		if (PawnStateInstances[i] == PawnStateInstance)
		{
			PAWNSTATE_LOG(Log, TEXT("%s: %s"), *FString(__FUNCTION__), *PawnStateInstances[i].ToString());

			FPawnStateInstance Instance(PawnStateInstances[i]);
			Instance.Instigator = Instigator;

			if (AbilitySystemComponent)
			{
				AbilitySystemComponent->UpdateTagMap(Instance.PawnStateTag, -1);
			}

			UPawnStateEvent* Event = GetLeaveEventByTag(PawnStateInstance.PawnStateTag);
			if (Event && Event->Delegate.IsBound())
			{
				Event->Delegate.Broadcast(Instance);
			}

			PawnStateInstances.RemoveAt(i);
			RebuildCurrentTag();

			break;
		}
	}
	return true;
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
