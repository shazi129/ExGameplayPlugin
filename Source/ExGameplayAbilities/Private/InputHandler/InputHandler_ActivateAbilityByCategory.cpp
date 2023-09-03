#include "InputHandler/InputHandler_ActivateAbilityByCategory.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ExGameplayAbilitiesModule.h"
#include "ExAbilitySystemComponent.h"

void UInputHandler_ActivateAbilityByCategory::NativeExecute(const FInputActionValue& inputValue)
{
	if (!CategoryTag.IsValid())
	{
		EXABILITY_LOG(Error, TEXT("%s error, CategoryTag is Invalid"), *FString(__FUNCTION__));
		return;
	}

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		EXABILITY_LOG(Error, TEXT("%s error, cannot find Character"), *FString(__FUNCTION__));
		return;
	}

	UExAbilitySystemComponent* ASC = Cast<UExAbilitySystemComponent>(Character->GetComponentByClass(UExAbilitySystemComponent::StaticClass()));
	if (!ASC)
	{
		EXABILITY_LOG(Error, TEXT("%s error, character has no UExAbilitySystemComponent"), *FString(__FUNCTION__));
		return;
	}

	ASC->ActivateAbilityByCategory(CategoryTag);
}
