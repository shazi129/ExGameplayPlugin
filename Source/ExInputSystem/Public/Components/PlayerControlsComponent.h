#pragma once

#include "CoreMinimal.h"
#include "InputActionHandler.h"
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

	UFUNCTION(Server, reliable, WithValidation)
	void ServerTriggerInputTag(const FGameplayTag& InputTag);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Controls")
	bool SetupInputWhenPawnStart = true;

	/*绑定输入配置*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Controls")
	TArray<UInputMappingConfigAsset*> InputMappingConfigList;

protected:
	
	/** Native/BP Event to set up player controls */
	UFUNCTION(BlueprintNativeEvent, Category = "Player Controls")
	void SetupPlayerControls(UEnhancedInputComponent* PlayerInputComponent);

	/** Native/BP Event to undo control setup */
	UFUNCTION(BlueprintNativeEvent, Category = "Player Controls")
	void TeardownPlayerControls(UEnhancedInputComponent* PlayerInputComponent);
	
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
	TArray<FInputMappingResult> InputMappingResultList;
};
