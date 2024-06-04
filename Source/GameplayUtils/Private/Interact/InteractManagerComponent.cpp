#include "Interact/InteractManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interact/InteractSubsystem.h"
#include "Actors/AbilityActor.h"

void UInteractManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (UInteractSubsystem* Subsystem = UInteractSubsystem::Get(this))
		{
			Subsystem->SetInteractPawn(OwnerPawn);
		}
	}
}

void UInteractManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}



void UInteractManagerComponent::ServerRequestStartInteract_Implementation(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName)
{
	UInteractSubsystem* InteractSubsystem = UInteractSubsystem::Get(this);
	if (!InteractSubsystem)
	{
		return;
	}

	if (!InteractSubsystem->CanInteract(InteractItem, Instigator, ConfigName))
	{
		return;
	}

	IInteractItemInterface::Execute_AddInteractingPawn(InteractItem.GetObject(), ConfigName, Instigator);

	//广播
	MulticastStartInteractItem(InteractItem, Instigator, ConfigName);
}

void UInteractManagerComponent::MulticastStartInteractItem_Implementation(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName)
{
	UInteractSubsystem* InteractSubsystem = UInteractSubsystem::Get(this);
	if (InteractSubsystem)
	{
		InteractSubsystem->InternalStartInteractItem(InteractItem, Instigator, ConfigName);
	}
}

void UInteractManagerComponent::ServerRequestStopInteract_Implementation(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName)
{
	UInteractSubsystem* InteractSubsystem = UInteractSubsystem::Get(this);
	if (!InteractSubsystem)
	{
		return;
	}

	IInteractItemInterface::Execute_RemoveInteractingPawn(InteractItem.GetObject(), ConfigName, Instigator);
	MulticastStopInteractItem(InteractItem, Instigator, ConfigName);
}

void UInteractManagerComponent::MulticastStopInteractItem_Implementation(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName)
{
	UInteractSubsystem* InteractSubsystem = UInteractSubsystem::Get(this);
	if (InteractSubsystem)
	{
		InteractSubsystem->InternalStopInteractItem(InteractItem, Instigator, ConfigName);
	}
}
