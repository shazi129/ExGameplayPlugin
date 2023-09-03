#include "ExGameplayAbility.h"

FExAbilityCase::FExAbilityCase(TSubclassOf<UGameplayAbility> InAbilityClass)
{
	AbilityClass = InAbilityClass;
}
