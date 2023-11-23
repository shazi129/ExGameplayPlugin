#include "ExGameplayAbility.h"
#include "ExGameplayAbilitiesModule.h"
#include "PawnStateSubsystem.h"
#include "PawnStateComponent.h"
#include "ExAbilitySystemComponent.h"
#include "PawnStateLibrary.h"
#include "ExGameplayAbilityLibrary.h"

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

bool UExGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (ActorInfo)
	{
		UExAbilitySystemComponent* ASC = Cast<UExAbilitySystemComponent>(ActorInfo->AbilitySystemComponent);
		if (ASC != nullptr && ASC->GetAbilityCooldown(GetClass()) > 0.0f)
		{
			return false;
		}
	}
	return Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
}

void UExGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (bApplyCooldownInEnd && !bInEndAbility)
	{
		return;
	}
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
}

bool UExGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	do
	{
		if (!ActorInfo)
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

		//检查pawnstate是否合法
		for (UPawnStateAsset* PawnStateAsset : AbilityStateAssets)
		{
			if (PawnStateSubsystem->LoadPawnStateAsset(PawnStateAsset))
			{
				FString  ErrorMsg;
				if (!PawnStateComponent->CanEnterPawnState(PawnStateAsset->StateTag, ErrorMsg))
				{
					EXABILITY_LOG(Log, TEXT("%s check %s failed: %s"), *FString(__FUNCTION__), *GetNameSafe(this), *ErrorMsg);
					return false;
				}
			}
		}

		//GAS的Block和PawnState的block互通
		for (const auto& GameplayTag : ActivationBlockedTags)
		{
			if (PawnStateComponent->HasPawnStateTag(GameplayTag))
			{
				EXABILITY_LOG(Log, TEXT("%s check %s failed: Activation blocked by %s"), *FString(__FUNCTION__), *GetNameSafe(this), *GameplayTag.ToString());
				return false;
			}
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

	ClearPawnState();

	//进入PawnState
	UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(ActorInfo->AvatarActor.Get());
	if (PawnStateComponent)
	{
		for (UPawnStateAsset* PawnStateAsset : AbilityStateAssets)
		{
			if (PawnStateAsset && PawnStateAsset->StateTag.IsValid())
			{
				//进入PawnState
				int PawnStateId = PawnStateComponent->InternalEnterPawnState(PawnStateAsset->StateTag, this, nullptr);
				if (PawnStateId > 0)
				{
					PawnStateIdList.Add(PawnStateId);
				}
			}
		}
	}
	return true;
}

void UExGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bInActivateAbility = true;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UExGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bInEndAbility = true;

	UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(ActorInfo->AvatarActor.Get());
	if (PawnStateComponent)
	{
		for (int PawnStateId : PawnStateIdList)
		{
			PawnStateComponent->LeavePawnState(PawnStateId, nullptr);
		}
		ClearPawnState();
	}

	if (bApplyCooldownInEnd)
	{
		ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	bInEndAbility = false;
}

void UExGameplayAbility::ClearPawnState()
{
	PawnStateIdList.Reset(AbilityStateAssets.Num());
}


