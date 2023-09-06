#include "Tasks/AbilityTask_ActivateAndWaitAbilityEnd.h"
#include "ExGameplayAbilitiesModule.h"


UAbilityTask_ActivateAndWaitAbilityEnd* UAbilityTask_ActivateAndWaitAbilityEnd::ActivateAndWaitAbilityEnd(UGameplayAbility* OwningAbility, const FExAbilityCase& AbilityCase, UObject* SourceObject, bool RemoveWhenEnd, int WaitFrames)
{
	UAbilityTask_ActivateAndWaitAbilityEnd* Task = NewAbilityTask<UAbilityTask_ActivateAndWaitAbilityEnd>(OwningAbility);
	Task->AbilityCase = AbilityCase;
	Task->AbilityCase.SourceObject = SourceObject;
	Task->RemoveWhenEnd = RemoveWhenEnd;
	Task->WaitFrames = WaitFrames;
	Task->ASC = nullptr;
	return Task;
}

void UAbilityTask_ActivateAndWaitAbilityEnd::Activate()
{
	if (!AbilityCase.IsValid())
	{
		EXABILITY_LOG(Error, TEXT("%s error, Ability Case invalide"));
		ActivateFailedDelegate.Broadcast(nullptr);
	}

	ASC = Cast<UExAbilitySystemComponent>(Ability->GetAbilitySystemComponentFromActorInfo());
	if (!ASC)
	{
		EXABILITY_LOG(Error, TEXT("%s error, cannot get UExAbilitySystemComponent from actor info"));
		ActivateFailedDelegate.Broadcast(nullptr);
		return;
	}

	Super::Activate();
	AbilityFailedDelegateHandle = ASC->AbilityFailedCallbacks.AddUObject(this, &UAbilityTask_ActivateAndWaitAbilityEnd::OnAbilityFailed);
	AbilityEndDelegateHandle = ASC->AbilityEndedCallbacks.AddUObject(this, &UAbilityTask_ActivateAndWaitAbilityEnd::OnAbilityEnd);
}


void UAbilityTask_ActivateAndWaitAbilityEnd::OnDestroy(bool AbilityEnded)
{
	if (ASC)
	{
		ASC->AbilityFailedCallbacks.Remove(AbilityFailedDelegateHandle);
		ASC->AbilityEndedCallbacks.Remove(AbilityEndDelegateHandle);
	}
}

bool UAbilityTask_ActivateAndWaitAbilityEnd::DoTaskWork()
{
	//是否已经give了
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityCase.AbilityClass);
	if (Spec)
	{
		ASC->TryActivateAbilityByCase(AbilityCase);
	}
	else
	{
		ASC->GiveAbilityByCase(AbilityCase, true);
	}
	return true;
}

void UAbilityTask_ActivateAndWaitAbilityEnd::OnAbilityFailed(const UGameplayAbility* InAbility, const FGameplayTagContainer& TagContainer)
{
	if (InAbility->GetClass() == AbilityCase.AbilityClass)
	{
		ActivateFailedDelegate.Broadcast(InAbility);
		EndTask();
	}
}

void UAbilityTask_ActivateAndWaitAbilityEnd::OnAbilityEnd(UGameplayAbility* InAbility)
{
	if (InAbility->GetClass() == AbilityCase.AbilityClass)
	{
		if (RemoveWhenEnd)
		{
			ASC->ClearAbilityByClass(AbilityCase.AbilityClass);
		}

		AbilityEndDelegate.Broadcast(InAbility);
		EndTask();
	}
}
