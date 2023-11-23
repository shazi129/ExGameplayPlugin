#include "ExGameplayModMagnitudeCalculation.h"
#include "AbilitySystemComponent.h"

float UExGameplayModMagnitudeCalculation::GetOwnerAttributeMagnitude(const FGameplayEffectSpec& EffectSpec, FGameplayAttribute Attribute) const
{
	const UAbilitySystemComponent* ASC = EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!ASC)
	{
		return 0.0f;
	}

	bool HasAttribute = false;
	float Magnitude = ASC->GetGameplayAttributeValue(Attribute, HasAttribute);
	if (!HasAttribute)
	{
		return 0.0f;
	}

	return Magnitude;
}
