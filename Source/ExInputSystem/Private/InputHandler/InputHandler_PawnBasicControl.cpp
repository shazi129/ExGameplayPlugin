#include "InputHandler/InputHandler_PawnBasicControl.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UInputHandler_PawnBasicControl::SetSourceObject(UObject* Object)
{
	Super::SetSourceObject(Object);

	if (SourceObject != nullptr)
	{
		if (SourceObject->IsA(UActorComponent::StaticClass()))
		{
			Pawn = Cast<APawn>(Cast<UActorComponent>(SourceObject)->GetOwner());
		}
		else
		{
			Pawn = Cast<APawn>(SourceObject);
		}
	}
	else
	{
		Pawn = nullptr;
	}
}

void UInputHandler_PawnMoveForward::NativeExecute(const FInputActionValue& inputValue)
{
	if (Pawn && Pawn->Controller && inputValue.GetMagnitude() != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = Pawn->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		Pawn->AddMovementInput(Direction, inputValue.GetMagnitude());
	}
}

void UInputHandler_PawnMoveRight::NativeExecute(const FInputActionValue& inputValue)
{
	if (Pawn && Pawn->Controller && inputValue.GetMagnitude() != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = Pawn->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		Pawn->AddMovementInput(Direction, inputValue.GetMagnitude());
	}
}

void UInputHandler_PawnMoveUp::NativeExecute(const FInputActionValue& inputValue)
{
	if (Pawn && Pawn->Controller && inputValue.GetMagnitude() != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = Pawn->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
		// add movement in that direction
		Pawn->AddMovementInput(Direction, inputValue.GetMagnitude());
	}
}

void UInputHandler_PawnYawInput::NativeExecute(const FInputActionValue& inputValue)
{
	if (Pawn && inputValue.GetMagnitude() != 0.0f)
	{
		Pawn->AddControllerYawInput(inputValue.GetMagnitude());
	}
}

void UInputHandler_PawnPitchInput::NativeExecute(const FInputActionValue& inputValue)
{
	if (Pawn && inputValue.GetMagnitude() != 0.0f)
	{
		Pawn->AddControllerPitchInput(inputValue.GetMagnitude());
	}
}

void UInputHandler_CharacterJump::NativeExecute(const FInputActionValue& inputValue)
{
	if (Pawn)
	{
		ACharacter* Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			Character->Jump();
		}
	}
}




