#pragma once

#include "ExInputTypes.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ExInputLibrary.generated.h"

UCLASS()
class EXINPUTSYSTEM_API UExInputLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="ExInputLibrary")
	static FInputMappingResult AddInputMappingConfig(FInputMappingConfig InputMappingConfig, UEnhancedInputComponent* InputComponent, UEnhancedInputLocalPlayerSubsystem* SubSystem, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable, Category = "ExInputLibrary")
	static void RemoveInputMappingConfig(const FInputMappingResult& FInputMappingResult, UEnhancedInputComponent* InputComponent, UEnhancedInputLocalPlayerSubsystem* Subsystem);
};