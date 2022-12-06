#include "ExLocalPlayerInputSubsystem.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "ExInputSystemModule.h"
#include "GameFramework/InputSettings.h"
#include "InputActionHandler.h"

UExLocalPlayerInputSubsystem* UExLocalPlayerInputSubsystem::GetSubsystem( UObject* WorldContextObject)
{
	auto Subsystem = USubsystemBlueprintLibrary::GetLocalPlayerSubsystem(WorldContextObject, UExLocalPlayerInputSubsystem::StaticClass());
	if (!Subsystem || !Subsystem->IsA(UExLocalPlayerInputSubsystem::StaticClass()))
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UExLocalPlayerInputSubsystem::GetSubsystem error, Cannot Get Subsystem by object:"), *GetNameSafe(WorldContextObject));
		return nullptr;
	}

	return Cast<UExLocalPlayerInputSubsystem>(Subsystem);
}

void UExLocalPlayerInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//InitializeInputComponent();
}
void UExLocalPlayerInputSubsystem::Deinitialize()
{
	Super::Deinitialize();
	DeinitializeInputComponent();
}

APlayerController* UExLocalPlayerInputSubsystem::GetPlayerController()
{
	ULocalPlayer* LocalPlayer = this->GetLocalPlayer();
	if (LocalPlayer == nullptr)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UExLocalPlayerInputSubsystem::GetPlayerController error, Cannot Get LocalPlayer"));
		return nullptr;
	}
	return LocalPlayer->GetPlayerController(LocalPlayer->GetWorld());
}

UEnhancedInputLocalPlayerSubsystem* UExLocalPlayerInputSubsystem::GetEnhancedInputSubsystem() const
{
	ULocalPlayer* LocalPlayer = this->GetLocalPlayer();
	if (LocalPlayer == nullptr)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UExLocalPlayerInputSubsystem::GetEnhancedInputSubsystem error, Cannot Get LocalPlayer"));
		return nullptr;
	}

	return LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
}

void UExLocalPlayerInputSubsystem::InitializeInputComponent()
{
	if (InputComponent == nullptr)
	{
		UClass* DefaultClass = UInputSettings::GetDefaultInputComponentClass();
		if (!DefaultClass->ClassDefaultObject->IsA(UEnhancedInputComponent::StaticClass()))
		{
			DefaultClass = UEnhancedInputComponent::StaticClass();
		}

		if (APlayerController* PlayerController = GetPlayerController())
		{
			const FName InputComponentName(TEXT("LocalPlayerInputComponent"));
			InputComponent = Cast<UEnhancedInputComponent>(NewObject<UInputComponent>(this, DefaultClass, InputComponentName));
			PlayerController->PushInputComponent(InputComponent);
		}
		else
		{
			EXINPUTSYSTEM_LOG(Error, TEXT("UExLocalPlayerInputSubsystem::InitializeInputComponent error, Cannot Get PlayerController"));
		}
	}
}

void UExLocalPlayerInputSubsystem::DeinitializeInputComponent()
{
	if (InputComponent)
	{
		if (APlayerController* PlayerController = GetPlayerController())
		{
			PlayerController->PopInputComponent(InputComponent);
		}
		else
		{
			EXINPUTSYSTEM_LOG(Error, TEXT("UExLocalPlayerInputSubsystem::InitializeInputComponent error, Cannot Get PlayerController"));
		}
	}
}

FInputMappingResult UExLocalPlayerInputSubsystem::AddInputMappingConfig(FInputMappingConfig InputMappingConfig, UObject* SouceObject)
{
	InitializeInputComponent();
	return UExInputLibrary::AddInputMappingConfig(InputMappingConfig, InputComponent, GetEnhancedInputSubsystem(), SouceObject);
}


void UExLocalPlayerInputSubsystem::RemoveInputMappingConfig(const FInputMappingResult& InputMappingResult)
{
	UExInputLibrary::RemoveInputMappingConfig(InputMappingResult, InputComponent, GetEnhancedInputSubsystem());
}

UEnhancedInputComponent* UExLocalPlayerInputSubsystem::GetInputComponent()
{
	return InputComponent;
}