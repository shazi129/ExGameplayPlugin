// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractItemComponent.h"
#include "Components/InteractManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ExGameplayLibrary.h"

FInteractData::FInteractData(AActor* Initiator, UInteractItemComponent* InteractComponent, const FGameplayTag& InteractType)
	: Initiator(Initiator)
	, InteractItemComponent(InteractComponent)
	, InteractType(InteractType)
{
	InteractState = E_Interact_None;
	InteractOrder = -1;
	NeedNofity = false;
	Enable = true;
}

FInteractConfigData* UInteractItemComponent::GetConfigData(const FGameplayTag& InteractType)
{
	for (int i = 0; i < InteractConfigList.Num(); i++)
	{
		if (InteractConfigList[i].InteractType == InteractType)
		{
			return &InteractConfigList[i];
		}
	}
	return nullptr;
}


// Sets default values for this component's properties
UInteractItemComponent::UInteractItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractItemComponent::BeginPlay()
{
	Super::BeginPlay();

	UInteractManagerComponent* InteractManagerComponent = GetInteractManagerComponent();
	if (!InteractManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractItemComponent::UpdateInteractState: cannot get manager"));
		return;
	}

	for (int i = 0; i < InteractConfigList.Num(); i++)
	{
		InteractManagerComponent->FindOrAddItem(this, InteractConfigList[i].InteractType);
	}
}

UInteractManagerComponent* UInteractItemComponent::GetInteractManagerComponent()
{
	//只在模拟端运行
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractItemComponent::GetInteractManagerComponent: cannot get player controller: [%s]"), *GetOwner()->GetName());
		return nullptr;
	}

	if (!UExGameplayLibrary::IsClient(PC))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractItemComponent::GetInteractManagerComponent: not in client"));
		return nullptr;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	APawn* InteractPawn = PC->GetPawn();
	if (!InteractPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractItemComponent::GetInteractManagerComponent: cannot get pawn: [%s]"), *GetOwner()->GetName());
		return nullptr;
	}

	UInteractManagerComponent* InteractManagerComponent = Cast<UInteractManagerComponent>(InteractPawn->GetComponentByClass(UInteractManagerComponent::StaticClass()));
	if (!InteractManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractItemComponent::GetInteractManagerComponent: cannot get manager"));
		return nullptr;
	}

	return InteractManagerComponent;
}

void UInteractItemComponent::NotifyInteractStateChange(const FInteractData& InteractData)
{
	BP_NotifyInteractStateChange(InteractData);

	OnInteractStateChange.Broadcast(InteractData);
}

void UInteractItemComponent::BP_NotifyInteractStateChange_Implementation(const FInteractData& InteractData)
{

}

bool UInteractItemComponent::CanInteract(const FInteractData& InteractData)
{
	if (CanInteractDelegate.IsBound())
	{
		return CanInteractDelegate.Execute(InteractData);
	}

	return BP_CanInteract(InteractData);
}

bool UInteractItemComponent::BP_CanInteract_Implementation(const FInteractData& InteractData)
{
	return InteractData.InteractState == E_Interact_Interactive && InteractData.InteractOrder == 0;
}

bool UInteractItemComponent::OnInteract(const FInteractData& InteractData)
{
	if (!CanInteract(InteractData))
	{
		return false;
	}

	OnInteractDelegate.Broadcast(InteractData);
	BP_OnInteract(InteractData);

	return true;
}

void UInteractItemComponent::BP_OnInteract_Implementation(const FInteractData& InteractData)
{

}

void UInteractItemComponent::SetEnable(bool Enable, FGameplayTag InteractType)
{
	ENetRole Role = GetOwner()->GetLocalRole();
	if (Role == ENetRole::ROLE_Authority)
	{
		ClientSetEnable(Enable, InteractType);
	}
	else
	{
		InternalSetEnable(Enable, InteractType);
	}
}

void UInteractItemComponent::InternalSetEnable(bool Enable, FGameplayTag InteractType)
{
	UInteractManagerComponent* ManagerComponent = GetInteractManagerComponent();
	if (!ManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractItemComponent::InternalSetEnable: cannot get manager component"));
		return;
	}

	ManagerComponent->SetItemEnable(this, InteractType, Enable);
}

bool UInteractItemComponent::ClientSetEnable_Validate(bool Enable, FGameplayTag InteractType)
{
	return true;
}

void UInteractItemComponent::ClientSetEnable_Implementation(bool Enable, FGameplayTag InteractType)
{
	InternalSetEnable(Enable, InteractType);
}

