#include "ExGameplayAbility.h"

bool UExGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!OverrideCheckCost)
	{
		return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	}
	return true;
}

void UExGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	return Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}


FExAbilityCase::FExAbilityCase(TSubclassOf<UGameplayAbility> InAbilityClass)
{
	AbilityClass = InAbilityClass;
}

