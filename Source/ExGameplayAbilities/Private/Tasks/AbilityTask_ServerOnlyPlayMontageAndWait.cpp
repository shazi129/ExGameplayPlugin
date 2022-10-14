#include "Tasks/AbilityTask_ServerOnlyPlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "ExAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UAbilityTask_ServerOnlyPlayMontageAndWait* UAbilityTask_ServerOnlyPlayMontageAndWait::CreateServerOnlyPlayMontageAndWaitProxy(
	UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* ServerOnlyMontage,
	float Rate, FName StartSection, bool bStopWhenAbilityEnds,
	float AnimRootMotionTranslationScale, float StartTimeSeconds)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UAbilityTask_ServerOnlyPlayMontageAndWait* MyObj = NewAbilityTask<UAbilityTask_ServerOnlyPlayMontageAndWait>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = ServerOnlyMontage;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->StartTimeSeconds = StartTimeSeconds;

	return MyObj;
}

void UAbilityTask_ServerOnlyPlayMontageAndWait::Activate()
{
	Super::Activate();

	UExAbilitySystemComponent* ASC = Cast<UExAbilitySystemComponent>(AbilitySystemComponent);
	if (ASC)
	{
		UAnimMontage* CurrentMontage = ASC->GetCurrentMontage();
		UGameplayAbility* AnimationAbility = ASC->GetAnimatingAbility();

		//当前正在播动画，RPC到Client
		if (CurrentMontage == MontageToPlay && AnimationAbility && AnimationAbility == Ability)
		{
			ASC->ClientPlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection, StartTimeSeconds);
		}
	}
}

void UAbilityTask_ServerOnlyPlayMontageAndWait::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
	UExAbilitySystemComponent* ASC = Cast<UExAbilitySystemComponent>(AbilitySystemComponent);
	if (ASC)
	{
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			ASC->ClientStopMontage();
		}
	}
}