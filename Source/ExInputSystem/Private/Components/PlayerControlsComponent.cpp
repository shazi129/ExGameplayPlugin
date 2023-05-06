// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/PlayerControlsComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "Engine/InputDelegateBinding.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "ExInputSystemModule.h"
#include "ExInputLibrary.h"
#include "ExInputSubsystem.h"

void UPlayerControlsComponent::OnRegister()
{
	Super::OnRegister();

	UWorld* World = GetWorld();
	APawn* MyOwner = GetPawn<APawn>();

	if (ensure(MyOwner) && World->IsGameWorld())
	{
		MyOwner->ReceiveRestartedDelegate.AddDynamic(this, &UPlayerControlsComponent::OnPawnRestarted);
		MyOwner->ReceiveControllerChangedDelegate.AddDynamic(this, &UPlayerControlsComponent::OnControllerChanged);

		// If our pawn has an input component we were added after restart
		if (MyOwner->InputComponent)
		{
			OnPawnRestarted(MyOwner);
		}
	}
}

void UPlayerControlsComponent::OnUnregister()
{
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
	{
		ReleaseInputComponent();

		APawn* MyOwner = GetPawn<APawn>();
		if (MyOwner)
		{
			MyOwner->ReceiveRestartedDelegate.RemoveAll(this);
			MyOwner->ReceiveControllerChangedDelegate.RemoveAll(this);
		}
	}

	Super::OnUnregister();
}

bool UPlayerControlsComponent::ServerTriggerInputTag_Validate(const FGameplayTag& InputTag)
{
	return true;
}

void  UPlayerControlsComponent::ServerTriggerInputTag_Implementation(const FGameplayTag& InputTag)
{
	UExInputSubsystem::TriggerInputTag(this, InputTag);	
}

void UPlayerControlsComponent::OnPawnRestarted(APawn* Pawn)
{
	if (ensure(Pawn && Pawn == GetOwner()) && Pawn->InputComponent)
	{
		ReleaseInputComponent();

		if (Pawn->InputComponent && SetupInputWhenPawnStart)
		{
			SetupInputComponent(Pawn);
		}
	}
}

void UPlayerControlsComponent::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	// Only handle releasing, restart is a better time to handle binding
	if (ensure(Pawn && Pawn == GetOwner()) && OldController)
	{
		ReleaseInputComponent(OldController);
	}
}

void UPlayerControlsComponent::SetupInputComponent(APawn* Pawn)
{
	InputComponent = CastChecked<UEnhancedInputComponent>(Pawn->InputComponent);
	if (InputComponent == nullptr)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UPlayerControlsComponent::SetupInputComponent error, Project must use EnhancedInputComponent to support PlayerControlsComponent"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem();
	for (int i = 0; i < InputMappingConfigList.Num(); i++)
	{
		UInputMappingConfigAsset* ConfigAsset = InputMappingConfigList[i];
		if (ConfigAsset && ConfigAsset->InputMappingConfig.InputMappingContext)
		{
			InputMappingResultList.Add(UExInputLibrary::AddInputMappingConfig(ConfigAsset->InputMappingConfig, InputComponent, Subsystem, this));
		}
	}

	SetupPlayerControls(InputComponent);
}

void UPlayerControlsComponent::ReleaseInputComponent(AController* OldController)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem(OldController);
	if (Subsystem && InputComponent)
	{
		TeardownPlayerControls(InputComponent);
		for (int i = 0; i < InputMappingResultList.Num(); i++)
		{
			UExInputLibrary::RemoveInputMappingConfig(InputMappingResultList[i], InputComponent, Subsystem);
		}
	}
	InputComponent = nullptr;
}

void UPlayerControlsComponent::SetupPlayerControls_Implementation(UEnhancedInputComponent* PlayerInputComponent)
{

}

void UPlayerControlsComponent::TeardownPlayerControls_Implementation(UEnhancedInputComponent* PlayerInputComponent)
{

}

UEnhancedInputLocalPlayerSubsystem* UPlayerControlsComponent::GetEnhancedInputSubsystem(AController* OldController) const
{
	const APlayerController* PC = GetController<APlayerController>();
	if (!PC)
	{
		PC = Cast<APlayerController>(OldController);
		if (!PC)
		{
			return nullptr;
		}
	}

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP)
	{
		return nullptr;
	}

	return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
}