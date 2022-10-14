#pragma once

#include "ExInputLibrary.h"
#include "ExLocalPlayerInputSubsystem.generated.h"

UCLASS(BlueprintType)
class EXINPUTSYSTEM_API UExLocalPlayerInputSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DefaultToSelf = "WorldContextObject"))
		static UExLocalPlayerInputSubsystem* GetSubsystem(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
		FInputMappingResult AddInputMappingConfig(FInputMappingConfig InputMappingConfig, UObject* SourceObject=nullptr);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
		void RemoveInputMappingConfig(const FInputMappingResult& FInputMappingResult);

	UFUNCTION(BlueprintCallable)
		UEnhancedInputComponent* GetInputComponent();

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	virtual void Deinitialize();

private:
	void InitializeInputComponent();
	void DeinitializeInputComponent();

	APlayerController* GetPlayerController();
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;

private:
	UPROPERTY(Transient, DuplicateTransient)
		TObjectPtr<UEnhancedInputComponent> InputComponent;
};
