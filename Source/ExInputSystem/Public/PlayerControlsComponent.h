#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputTriggers.h"
#include "ExInputTypes.h"
#include "Components/PawnComponent.h"
#include "PlayerControlsComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UEnhancedInputComponent;
class UInputMappingContext;
class UInputAction;

/** Modular pawn component for adding input actions and an optional input mapping to a pawn */
UCLASS(Blueprintable, BlueprintType, Category = "Input", meta = (BlueprintSpawnableComponent))
class EXINPUTSYSTEM_API UPlayerControlsComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	//~ Begin UActorComponent interface
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	//~ End UActorComponent interface

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Controls")
		bool SetupInputWhenPawnStart = true;

	/*∞Û∂® ‰»Î≈‰÷√*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Controls")
		FInputMappingConfig InputMappingConfig;

	bool isRegister = false;

protected:

	/** Native/BP Event to set up player controls */
	UFUNCTION(BlueprintNativeEvent, Category = "Player Controls")
		void SetupPlayerControls(UEnhancedInputComponent* PlayerInputComponent);

	/** Native/BP Event to undo control setup */
	UFUNCTION(BlueprintNativeEvent, Category = "Player Controls")
		void TeardownPlayerControls(UEnhancedInputComponent* PlayerInputComponent);

	/** Wrapper function for binding to this input component */
	template<class UserClass, typename FuncType>
	bool BindInputAction(const UInputAction* Action, const ETriggerEvent EventType, UserClass* Object, FuncType Func)
	{
		if (ensure(InputComponent != nullptr) && ensure(Action != nullptr))
		{
			InputComponent->BindAction(Action, EventType, Object, Func);
			return true;
		}

		return false;
	}

	/** Called when pawn restarts, bound to dynamic delegate */
	UFUNCTION()
		virtual void OnPawnRestarted(APawn* Pawn);

	/** Called when pawn restarts, bound to dynamic delegate */
	UFUNCTION()
		virtual void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	virtual void SetupInputComponent(APawn* Pawn);
	virtual void ReleaseInputComponent(AController* OldController = nullptr);
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem(AController* OldController = nullptr) const;

protected:

	/** The bound input component. */
	UPROPERTY(transient)
		UEnhancedInputComponent* InputComponent;

	UPROPERTY()
		FInputMappingResult InputMappingResult;
};
