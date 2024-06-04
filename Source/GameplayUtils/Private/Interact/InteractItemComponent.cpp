#include "Interact/InteractItemComponent.h"
#include "Interact/InteractSubsystem.h"
#include "Net/UnrealNetwork.h"

UInteractItemComponent::UInteractItemComponent()
{
	//不需要tick
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractItemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UInteractSubsystem* Subsystem = UInteractSubsystem::Get(this))
	{
		Subsystem->RegisterInteractItem(this, GetOwner());
	}

	for (auto& HandlerItem : StateChangeHandlerMap)
	{
		if (HandlerItem.Value)
		{
			HandlerItem.Value->SetSourceObject(this);
		}
	}

	for (auto& HandlerItem : InteractHandlerMap)
	{
		if (HandlerItem.Value)
		{
			HandlerItem.Value->SetSourceObject(this);
		}
	}
}

void UInteractItemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UInteractSubsystem* Subsystem = UInteractSubsystem::Get(this))
	{
		Subsystem->UnregisterInteractItem(this);
	}
}

void UInteractItemComponent::SetEnable(bool Enable, const FName& ConfigName)
{
	if (UInteractSubsystem* Subsystem = UInteractSubsystem::Get(this))
	{
		Subsystem->SetItemEnable(this, ConfigName, Enable);
	}
}

void UInteractItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UInteractItemComponent, InteractingInfoList, COND_None)
}

void UInteractItemComponent::OnInteractStateChange_Implementation(const FInteractInstanceData& InteractData)
{
	if (InteractStateChangeDelegate.IsBound())
	{
		InteractStateChangeDelegate.Broadcast(InteractData);
	}
}

bool UInteractItemComponent::CanInteract_Implementation(const FInteractInstanceData& InteractData)
{
	if (CanInteractDelegate.IsBound())
	{
		return CanInteractDelegate.Execute(InteractData);
	}

	return true;
}

void UInteractItemComponent::StartInteract_Implementation(const FInteractInstanceData& InteractData)
{
	if (StartInteractDelegate.IsBound())
	{
		StartInteractDelegate.Broadcast(InteractData);
	}
}

int32 UInteractItemComponent::GetInteractingNum_Implementation(FName const& ConfigName)
{
	return InteractingInfoList.Num();
}

void UInteractItemComponent::AddInteractingPawn_Implementation(FName const& ConfigName, APawn* Pawn)
{
	if (GetOwner()->HasAuthority())
	{
		bool HasFound = false;
		for (auto& InteractingInfo : InteractingInfoList)
		{
			if (InteractingInfo.ConfigName == InteractingInfo.ConfigName && InteractingInfo.Instigator == Pawn)
			{
				HasFound = true;
			}
		}
		if (!HasFound)
		{
			auto* NewInteractingInfo = new (InteractingInfoList) FInteractReplicateData();
			NewInteractingInfo->ConfigName = ConfigName;
			NewInteractingInfo->Instigator = Pawn;
		}
	}
}

void UInteractItemComponent::RemoveInteractingPawn_Implementation(const FName& ConfigName, APawn* Pawn)
{
	if (GetOwner()->HasAuthority())
	{
		for (int i = InteractingInfoList.Num() - 1; i >= 0; i--)
		{
			FInteractReplicateData& InteractingInfo = InteractingInfoList[i];
			if (InteractingInfo.ConfigName == InteractingInfo.ConfigName && InteractingInfo.Instigator == Pawn)
			{
				InteractingInfoList.RemoveAt(i);
				return;
			}
		}
	}
}

UInteractItemHandler* UInteractItemComponent::GetStateChangeHandler_Implementation(const FName& ConfigName)
{
	return *StateChangeHandlerMap.Find(ConfigName.ToString());
}

UInteractItemHandler* UInteractItemComponent::GetInteractHandler_Implementation(const FName& ConfigName)
{
	return *InteractHandlerMap.Find(ConfigName.ToString());
}




