#pragma once

/**
 *  这里面包含这一些Pawn基本控制的一些input handler
 */

#include "CoreMinimal.h"
#include "InputBindingAction.h"
#include "InputHandler_PawnBasicControl.generated.h"

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveForward : public UInputBindingActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveRight : public UInputBindingActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnYawInput : public UInputBindingActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnPitchInput : public UInputBindingActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveUp : public UInputBindingActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};