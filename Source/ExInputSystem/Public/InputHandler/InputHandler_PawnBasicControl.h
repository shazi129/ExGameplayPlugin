#pragma once

/**
 *  这里面包含这一些Pawn基本控制的一些input handler
 */

#include "CoreMinimal.h"
#include "InputActionHandler.h"
#include "InputHandler_PawnBasicControl.generated.h"

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnBasicControl : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void SetSourceObject(UObject* Object);

protected:
	APawn* Pawn;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveForward : public UInputHandler_PawnBasicControl
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveRight : public UInputHandler_PawnBasicControl
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnYawInput : public UInputHandler_PawnBasicControl
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnPitchInput : public UInputHandler_PawnBasicControl
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_PawnMoveUp : public UInputHandler_PawnBasicControl
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};

UCLASS()
class EXINPUTSYSTEM_API UInputHandler_CharacterJump : public UInputHandler_PawnBasicControl
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};