#include "ExInputLibrary.h"
#include "ExInputSystemModule.h"
#include "Kismet/GameplayStatics.h"

FInputMappingResult UExInputLibrary::AddInputMappingConfig(FInputMappingConfig InputMappingConfig, UEnhancedInputComponent* InputComponent, UEnhancedInputLocalPlayerSubsystem* Subsystem, UObject* SourceObject)
{
	FInputMappingResult Result;

	if (Subsystem == nullptr)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UExInputLibrary::AddInputMappingConfig error, Subsystem is null"));
		return Result;
	}
	if (InputComponent == nullptr)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UExInputLibrary::AddInputMappingConfig error, InputComponent is null"));
		return Result;
	}

	
	if (InputMappingConfig.InputMappingContext)
	{
		Subsystem->AddMappingContext(InputMappingConfig.InputMappingContext, InputMappingConfig.InputPriority);
		Result.InputMappingContext = InputMappingConfig.InputMappingContext;
	}

	for (int i = 0; i < InputMappingConfig.InputBindings.Num(); i++)
	{
		FInputBindingConfig& BindingConfig = InputMappingConfig.InputBindings[i];
		if (BindingConfig.InputAction != nullptr && BindingConfig.InputHandler != nullptr)
		{
			BindingConfig.InputHandler->SetSourceObject(SourceObject);

			FEnhancedInputActionEventBinding& Binding = InputComponent->BindAction(
				BindingConfig.InputAction,
				BindingConfig.TriggerEvent,
				BindingConfig.InputHandler,
				&UInputActionHandler::NativeExecute);

			int BindingHandle = Binding.GetHandle();
			Result.InputBindHandlers.AddUnique(BindingHandle);
		}
	}

	return Result;
}

void UExInputLibrary::RemoveInputMappingConfig(const FInputMappingResult& FInputMappingResult, UEnhancedInputComponent* InputComponent, UEnhancedInputLocalPlayerSubsystem* Subsystem)
{
	if (Subsystem == nullptr)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UExInputLibrary::RemoveInputMappingConfig error, Subsystem is null"));
		return;
	}
	if (InputComponent == nullptr)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UExInputLibrary::RemoveInputMappingConfig error, InputComponent is null"));
		return;
	}

	if (FInputMappingResult.InputMappingContext)
	{
		Subsystem->RemoveMappingContext(FInputMappingResult.InputMappingContext);
	}

	for (int i = 0; i < FInputMappingResult.InputBindHandlers.Num(); i++)
	{
		bool Success = InputComponent->RemoveBindingByHandle(FInputMappingResult.InputBindHandlers[i]);
		if (!Success)
		{
			EXINPUTSYSTEM_LOG(Error, TEXT("UExInputLibrary::RemoveInputMappingConfig error, RemoveBindingByHandle[%d] error"), FInputMappingResult.InputBindHandlers[i]);
		}
	}
}
