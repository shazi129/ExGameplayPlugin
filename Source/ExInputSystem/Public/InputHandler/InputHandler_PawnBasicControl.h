#pragma once

/**
 *  这里面包含这一些Pawn基本控制的一些input handler
 */

#include "CoreMinimal.h"
#include "InputActionHandler.h"
#include "InputHandler_PawnBasicControl.generated.h"

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveForward : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveRight : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnYawInput : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnPitchInput : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveUp : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};