#include "InputHandler/InputHandler_PawnBasicControl.h"
#include "Kismet/GameplayStatics.h"

void UInputHandler_PawnMoveForward::NativeExecute(const FInputActionValue& inputValue)
{
	if (APawn* Pawn = Cast<APawn>(GetSourceObject()))
	{
		if ((Pawn->Controller != nullptr) && (inputValue.GetMagnitude() != 0.0f))
		{
			// find out which way is forward
			const FRotator Rotation = Pawn->Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			Pawn->AddMovementInput(Direction, inputValue.GetMagnitude());
		}
	}
}

void UInputHandler_PawnMoveRight::NativeExecute(const FInputActionValue& inputValue)
{
	if (APawn* Pawn = Cast<APawn>(GetSourceObject()))
	{
		if ((Pawn->Controller != nullptr) && (inputValue.GetMagnitude() != 0.0f))
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
}

void UInputHandler_PawnMoveUp::NativeExecute(const FInputActionValue& inputValue)
{

	if (APawn* Pawn = Cast<APawn>(GetSourceObject()))
	{
		if ((Pawn->Controller != nullptr) && (inputValue.GetMagnitude() != 0.0f))
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
}

void UInputHandler_PawnYawInput::NativeExecute(const FInputActionValue& inputValue)
{
	if (APawn* Pawn = Cast<APawn>(GetSourceObject()))
	{
		Pawn->AddControllerYawInput(inputValue.GetMagnitude());
	}
}

void UInputHandler_PawnPitchInput::NativeExecute(const FInputActionValue& inputValue)
{
	if (APawn* Pawn = Cast<APawn>(GetSourceObject()))
	{
		Pawn->AddControllerPitchInput(inputValue.GetMagnitude());
	}
}




