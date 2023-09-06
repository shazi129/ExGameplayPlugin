#include "Props/PropsItemComponent.h"

void UPropsItemComponent::CollectAbilitCases(TArray<FExAbilityCase>& Abilities) const
{
	Abilities.Append(PropsAbilities);
}

FAbilityProviderInfo UPropsItemComponent::GetProviderInfo()
{
	if (InstancedProps)
	{
		return FAbilityProviderInfo(PropsInfo.PropsType, PropsInfo.PropsID, this);
	}
	else
	{
		return FAbilityProviderInfo(PropsInfo.PropsType, PropsInfo.PropsID, nullptr);
	}
}
