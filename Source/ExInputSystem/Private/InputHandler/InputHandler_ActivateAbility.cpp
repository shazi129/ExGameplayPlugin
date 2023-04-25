#include "InputHandler/InputHandler_ActivateAbility.h"
#include "ExInputSystemModule.h"

void UInputHandler_ActivateAbility::NativeExecute(const FInputActionValue& inputValue)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC && AbilityClass)
	{
		FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(AbilityClass);
		if (AbilitySpec)
		{
			ASC->TryActivateAbility(AbilitySpec->Handle);
		}
		else
		{
			EXINPUTSYSTEM_LOG(Error, TEXT("%s error, Not Given Ability[%s]"), *FString(__FUNCTION__), *GetNameSafe(AbilityClass));
		}
	}
	else
	{
		EXINPUTSYSTEM_LOG(Error, TEXT("%s error, Cannot get ASC From [%s]"), *FString(__FUNCTION__), *GetNameSafe(SourceObject));
	}
}

UAbilitySystemComponent* UInputHandler_ActivateAbility::GetAbilitySystemComponent()
{
	UActorComponent* Component = Cast<UActorComponent>(SourceObject);
	if (!Component)
	{
		return nullptr;
	}
	return Cast<UAbilitySystemComponent>(Component->GetOwner()->GetComponentByClass(UAbilitySystemComponent::StaticClass()));
}