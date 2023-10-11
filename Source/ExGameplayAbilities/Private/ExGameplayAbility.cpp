#include "ExGameplayAbility.h"
#include "ExGameplayAbilitiesModule.h"
#include "PawnStateSubsystem.h"
#include "PawnStateComponent.h"
#include "ExAbilitySystemComponent.h"
#include "PawnStateLibrary.h"

FExAbilityCase::FExAbilityCase(TSubclassOf<UGameplayAbility> InAbilityClass)
{
	AbilityClass = InAbilityClass;
}

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

bool UExGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	do
	{
		if (!ActorInfo || !AbilityStateAsset)
		{
			break;
		}

		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();
		UPawnStateComponent* PawnStateComponent = Cast<UPawnStateComponent>(AvatarActor->GetComponentByClass(UPawnStateComponent::StaticClass()));
		if (!PawnStateComponent)
		{
			break;
		}

		UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(PawnStateComponent);
		if (!PawnStateSubsystem)
		{
			break;
		}

		if (!PawnStateSubsystem->LoadPawnStateAsset(AbilityStateAsset))
		{
			break;
		}

		FString  ErrorMsg;
		if (!PawnStateComponent->CanEnterPawnState(AbilityStateAsset->StateTag, ErrorMsg))
		{
			EXABILITY_LOG(Log, TEXT("%s check %s failed: %s"), *FString(__FUNCTION__), *GetNameSafe(this), *ErrorMsg);
			return false;
		}
	}
	while(false);

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

bool UExGameplayAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{
 	if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
	{
		return false;
	}

	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	if (!PawnStateSubsystem || !ActorInfo || ActorInfo->AvatarActor == nullptr)
	{
		return true;
	}

	//进入PawnState
	if (AbilityStateAsset && AbilityStateAsset->StateTag.IsValid())
	{
		UPawnStateComponent* PawnStateComponent = Cast<UPawnStateComponent>(ActorInfo->AvatarActor->GetComponentByClass(UPawnStateComponent::StaticClass()));
		if (PawnStateComponent)
		{
			//进入PawnState
			PawnStateID = PawnStateComponent->InternalEnterPawnState(AbilityStateAsset->StateTag, this, nullptr);
		}
	}
	return true;
}

void UExGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (PawnStateID > 0)
	{
		UPawnStateComponent* PawnStateComponent = Cast<UPawnStateComponent>(ActorInfo->AvatarActor->GetComponentByClass(UPawnStateComponent::StaticClass()));
		if (PawnStateComponent)
		{
			PawnStateComponent->LeavePawnState(PawnStateID, nullptr);
		}
		ClearPawnState();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UExGameplayAbility::ClearPawnState()
{
	PawnStateID = 0;
}


