#include "InputHandler/InputHandler_TriggerGameAbility.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ExInputSystemModule.h"
#include "AbilitySystemComponent.h"

void UInputHandler_TriggerGameAbility::NativeExecute(const FInputActionValue& inputValue)
{
	if (!AbilityClass)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UInputHandler_TriggerGameAbility::Execute error, AbilityClass is Valid"));
		return;
	}

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UInputHandler_TriggerGameAbility::Execute error, cannot find Character"));
		return;
	}

	UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(Character->GetComponentByClass(UAbilitySystemComponent::StaticClass()));
	if (!ASC)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UInputHandler_TriggerGameAbility::Execute error, character has no UExAbilitySystemComponent"));
		return;
	}

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
	if (Spec)
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("UInputHandler_TriggerGameAbility::Execute error, character has no UExAbilitySystemComponent"));
		return;
	}

	ASC->TryActivateAbility(Spec->Handle);
}