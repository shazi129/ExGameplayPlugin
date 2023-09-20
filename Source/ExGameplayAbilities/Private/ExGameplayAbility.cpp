#include "ExGameplayAbility.h"
#include "ExGameplayAbilitiesModule.h"
#include "ExAbilitySystemComponent.h"

bool UExGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!OverrideCostCheck)
	{
		return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	}

	for (auto& CheckerClass : OverrideCostCheckers)
	{
		if (CheckerClass != nullptr)
		{
			UExGameplayAbilityChecker* CheckObject = CheckerClass->GetDefaultObject<UExGameplayAbilityChecker>();
			if (CheckObject->Check(this, *ActorInfo) == false)
			{
				return false;
			}
		}
	}
	return true;
}

void UExGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UExAbilitySystemComponent* ExASC = Cast<UExAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (ExASC)
	{
		ExASC->SetCurrentApplyCostAbility(this);
	}

	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	if (ExASC)
	{
		ExASC->SetCurrentApplyCostAbility(nullptr);
	}
}

FExAbilityCase::FExAbilityCase(TSubclassOf<UGameplayAbility> InAbilityClass)
{
	AbilityClass = InAbilityClass;
}


