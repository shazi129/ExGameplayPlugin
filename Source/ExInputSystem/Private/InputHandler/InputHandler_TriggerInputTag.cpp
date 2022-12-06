#include "InputHandler/InputHandler_TriggerInputTag.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ExInputSystemModule.h"
#include "Components/InteractManagerComponent.h"

void UInputHandler_TriggerInptutTag::NativeExecute(const FInputActionValue& inputValue)
{
	if (!InputTag.IsValid())
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UInputHandler_TriggerInptuTag::Execute error, InputTag is Valid"));
		return;
	}
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UInputHandler_TriggerInptuTag::Execute error, cannot find Character"));
		return;
	}

	UInteractManagerComponent* InteractManager = Cast<UInteractManagerComponent>(Character->GetComponentByClass(UInteractManagerComponent::StaticClass()));
	if (!InteractManager)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UInputHandler_TriggerInptuTag::Execute error, character has no UInteractManagerComponent"));
		return;
	}

	InteractManager->ReceiveInput(InputTag);
}