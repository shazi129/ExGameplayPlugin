#include "ExAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "ExGameplayAbilitiesModule.h"
#include "ExGameplayLibrary.h"
#include "ExAbilityProvider.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameplayCueManager.h"
#include "Engine/ActorChannel.h"

void UExAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	RegisterAbilityProvider(this);
}

void UExAbilitySystemComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	ClearAllProvider();
	Super::EndPlay(EndPlayReason);
}

void UExAbilitySystemComponent::BeginDestroy()
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor.IsValid())
	{
		if (UGameInstance* GameInstance = AbilityActorInfo->OwnerActor->GetGameInstance())
		{
			GameInstance->GetOnPawnControllerChanged().RemoveAll(this);
		}
	}
	Super::BeginDestroy();
}

void UExAbilitySystemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UExAbilitySystemComponent::OnPawnControllerChanged(APawn* Pawn, AController* NewController)
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor == Pawn && AbilityActorInfo->PlayerController != NewController)
	{
		// Reinit the cached ability actor info (specifically the player controller)
		AbilityActorInfo->InitFromActor(AbilityActorInfo->OwnerActor.Get(), AbilityActorInfo->AvatarActor.Get(), this);
	}
}

void UExAbilitySystemComponent::ClearAbilityByClassInternal(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityClass))
	{
		ClearAbility(Spec->Handle);
	}
}

void UExAbilitySystemComponent::ClearAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		ABILITY_LOG(Error, TEXT("%s error, ASC has no owner"), *FString(__FUNCTION__));
		return;
	}

	ENetRole LocalRole = Owner->GetLocalRole();
	ABILITY_LOG(Log, TEXT("%s in %d"), *FString(__FUNCTION__), LocalRole);

	if (LocalRole == ENetRole::ROLE_Authority)
	{
		return ClearAbilityByClassInternal(AbilityClass);
	}
	else if (LocalRole == ENetRole::ROLE_AutonomousProxy)
	{
		ServerClearAbilityByClass(AbilityClass);
	}
	else
	{
		ABILITY_LOG(Error, TEXT("%s cannot be used in role: %d"), *FString(__FUNCTION__), LocalRole);
	}
}

bool UExAbilitySystemComponent::ServerClearAbilityByClass_Validate(TSubclassOf<UGameplayAbility> AbilityClass)
{
	return true;
}

void UExAbilitySystemComponent::ServerClearAbilityByClass_Implementation(TSubclassOf<UGameplayAbility> AbilityClass)
{
	ClearAbilityByClassInternal(AbilityClass);
}

void UExAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
}

void UExAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);
}

void UExAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
}

void UExAbilitySystemComponent::ClientPlayMontage_Implementation(
		UGameplayAbility* AnimatingAbility,
		FGameplayAbilityActivationInfo ActivationInfo,
		UAnimMontage* Montage, float InPlayRate,
		FName StartSectionName, float StartTimeSeconds)
{
	//只在主控端播放
	ENetRole OwnerRole = GetOwner()->GetLocalRole();
	if (OwnerRole == ENetRole::ROLE_AutonomousProxy)
	{
		PlayMontage(AnimatingAbility, ActivationInfo, Montage, InPlayRate, StartSectionName, StartTimeSeconds);
	}
}


void  UExAbilitySystemComponent::ClientStopMontage_Implementation()
{
	ENetRole OwnerRole = GetOwner()->GetLocalRole();
	if (OwnerRole == ENetRole::ROLE_AutonomousProxy)
	{
		CurrentMontageStop();
	}
}


#pragma region //////////////////////////// Override AbilitySystemComponent

void UExAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (AbilityActorInfo)
	{
		if (AbilityActorInfo->AnimInstance == nullptr)
		{
			AbilityActorInfo->AnimInstance = AbilityActorInfo->GetAnimInstance();
		}

		if (UGameInstance* GameInstance = InOwnerActor->GetGameInstance())
		{
			// Sign up for possess/unpossess events so that we can update the cached AbilityActorInfo accordingly
			//GameInstance->GetOnPawnControllerChanged().AddDynamic(this, &UExAbilitySystemComponent::OnPawnControllerChanged);
		}
	}

	//GrantDefaultAbilitiesAndAttributes();
}

void UExAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);
}

void UExAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

bool UExAbilitySystemComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = UGameplayTasksComponent::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (const UAttributeSet* Set : GetSpawnedAttributes())
	{
		if (IsValid(Set))
		{
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UAttributeSet*>(Set), *Bunch, *RepFlags);
		}
	}

	for (UGameplayAbility* Ability : AllReplicatedInstancedAbilities)
	{
		if (IsValid(Ability))
		{
			UExGameplayAbility* ExAbility = Cast<UExGameplayAbility>(Ability);
			if (ExAbility && ExAbility->OnlyReplateWhenActivate)
			{
				FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(Ability->GetClass());
				if (Spec && Spec->IsActive())
				{
					WroteSomething |= Channel->ReplicateSubobject(Ability, *Bunch, *RepFlags);
				}
			}
			else
			{
				WroteSomething |= Channel->ReplicateSubobject(Ability, *Bunch, *RepFlags);
			}
		}
	}

	return WroteSomething;
}

#pragma endregion

#pragma region //////////////////////////// Provider 相关

void UExAbilitySystemComponent::CollectAbilitCases(TArray<FExAbilityCase>& Abilities) const
{
	for (const FExAbilityCase& Ability : DefaultAbilities)
	{
		Abilities.Add(Ability);
	}
}

void UExAbilitySystemComponent::RegisterAbilityProvider(TScriptInterface<IExAbilityProvider> Provider)
{
	UObject* ProviderObject = Cast<UObject>(Provider.GetObject());
	if (ProviderObject == nullptr)
	{
		EXABILITY_LOG(Error, TEXT("%s ignore, Provider is not a UObject"), *FString(__FUNCTION__));
		return;
	}

	FAbilityProviderInfo ProviderInfo = Provider->GetProviderInfo();
	for (const auto& CollectedAbilityInfo : CollectedAbilityInfoList)
	{
		if (CollectedAbilityInfo.ProviderInfo == ProviderInfo)
		{
			EXABILITY_LOG(Error, TEXT("%s error, duplicated provider: %s"), *FString(__FUNCTION__), *ProviderInfo.ToString());
			return;
		}
	}

	FCollectedAbilityInfo* NewCollectedAbilityInfo = nullptr;
	for (auto& CollectedAbilityInfo : CollectedAbilityInfoList)
	{
		if (!CollectedAbilityInfo.IsVaild())
		{
			NewCollectedAbilityInfo = &CollectedAbilityInfo;
			break;
		}
	}
	if (!NewCollectedAbilityInfo)
	{
		CollectedAbilityInfoList.Emplace();
		NewCollectedAbilityInfo = &CollectedAbilityInfoList.Last();
	}

	NewCollectedAbilityInfo->ProviderInfo = ProviderInfo;

	//收集技能
	Provider->CollectAbilitCases(NewCollectedAbilityInfo->AbilityCases);
	if (!NewCollectedAbilityInfo->AbilityCases.IsEmpty())
	{
		int RegisterTime = (int)(FPlatformTime::Seconds() * 1000.0f);
		for (FExAbilityCase& AbilityCase : NewCollectedAbilityInfo->AbilityCases)
		{
			AbilityCase.SourceObject = ProviderObject;
			for (auto& Category : AbilityCase.AbilityCategories)
			{
				Category.Priority = RegisterTime;
			}
		}

		//重建索引
		ReCollectedAbilityInfo();

		//只有Authority的时候才giveability， 防止give两次
		if (GetOwner()->HasAuthority())
		{
			for (FExAbilityCase& AbilityCase : NewCollectedAbilityInfo->AbilityCases)
			{
				GiveAbilityByCase(AbilityCase);
			}
		}
	}
}

void UExAbilitySystemComponent::UnregisterAbilityProvider(TScriptInterface<IExAbilityProvider> Provider)
{
	UObject* ProviderObject = Cast<UObject>(Provider.GetObject());
	if (!ProviderObject)
	{
		EXABILITY_LOG(Error, TEXT("%s ignore, Invalide SourceObject"), *FString(__FUNCTION__));
		return;
	}

	FAbilityProviderInfo ProviderInfo = Provider->GetProviderInfo();

	FCollectedAbilityInfo* UnregisterAbilities = nullptr;
	for (FCollectedAbilityInfo& CollectedAbilityInfo : CollectedAbilityInfoList)
	{
		if (CollectedAbilityInfo.ProviderInfo == ProviderInfo)
		{
			UnregisterAbilities = &CollectedAbilityInfo;
			break;
		}
	}

	if (!UnregisterAbilities)
	{
		EXABILITY_LOG(Warning, TEXT("%s ignore, cannot find provider:%s"), *FString(__FUNCTION__), *ProviderInfo.ToString());
		return;
	}

	ClearCollectedAbilities(*UnregisterAbilities, false);
	ReCollectedAbilityInfo();
}

void UExAbilitySystemComponent::ClearAllProvider()
{
	for (FCollectedAbilityInfo& Info : CollectedAbilityInfoList)
	{
		ClearCollectedAbilities(Info, false);
	}
	ReCollectedAbilityInfo();
}

void UExAbilitySystemComponent::ClearCollectedAbilities(FCollectedAbilityInfo& CollectedAbilityInfo, bool NeedRebuildCategory)
{
	//只有Authority的时候才clear ability， 防止clear两次
	if (GetOwner()->HasAuthority())
	{
		for (FExAbilityCase& AbilityCase : CollectedAbilityInfo.AbilityCases)
		{
			ClearAbilityByClass(AbilityCase.AbilityClass);
		}
	}

	CollectedAbilityInfo.Reset();
	if (NeedRebuildCategory)
	{
		ReCollectedAbilityInfo();
	}
}

void UExAbilitySystemComponent::ReCollectedAbilityInfo()
{
	RebuildAbilityCaseMap();
	RebuildAbilityCategory();
}

#pragma endregion

#pragma region /////////////////////////////// 技能分类相关
void UExAbilitySystemComponent::ActivateAbilityByCategory(const FGameplayTag& CategoryTag)
{
	if (!AbilityCategoryMap.Contains(CategoryTag))
	{
		EXABILITY_LOG(Error, TEXT("%s ignore, no ability actegory: %s"), *FString(__FUNCTION__), *CategoryTag.ToString());
		return;
	}

	TArray<FAbilityCategoryIndex>& CategoryIndexList = AbilityCategoryMap[CategoryTag];
	if (CategoryIndexList.Num() == 0)
	{
		EXABILITY_LOG(Error, TEXT("%s ignore, Empty actegory: %s"), *FString(__FUNCTION__), *CategoryTag.ToString());
		return;
	}

	FAbilityCategoryIndex& CategoryIndex = CategoryIndexList[0];
	if (CategoryIndex.AbilityCasePtr && CategoryIndex.AbilityCasePtr->CanActivate)
	{
		//如果没give， give一次
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromCase(*CategoryIndex.AbilityCasePtr);
		if (!Spec)
		{
			GiveAbilityByCase(*CategoryIndex.AbilityCasePtr, true);
		}
		else
		{
			TryActivateAbilityByCase(*CategoryIndex.AbilityCasePtr);
		}
	}
}

void UExAbilitySystemComponent::RebuildAbilityCategory()
{
	AbilityCategoryMap.Reset();

	for (FCollectedAbilityInfo& CollectedAbilityInfo : CollectedAbilityInfoList)
	{
		if (CollectedAbilityInfo.IsVaild() == false || CollectedAbilityInfo.AbilityCases.IsEmpty())
		{
			continue;
		}
		for (FExAbilityCase& AbilityCase : CollectedAbilityInfo.AbilityCases)
		{
			for (FAbilityCategory& AbilityCategory : AbilityCase.AbilityCategories)
			{
				auto& AbilityCasePtrList = AbilityCategoryMap.FindOrAdd(AbilityCategory.CategoryTag);

				AbilityCasePtrList.Emplace();
				FAbilityCategoryIndex& CategoryIndex = AbilityCasePtrList.Last();
				CategoryIndex.AbilityCasePtr = &AbilityCase;
				CategoryIndex.CategoryPriority = AbilityCategory.Priority;
			}
		}
	}

	//排序
	for (auto& AbilityCategoryItem : AbilityCategoryMap)
	{
		AbilityCategoryItem.Value.Sort([](const FAbilityCategoryIndex& A, const FAbilityCategoryIndex& B)
		{
				if (A.AbilityCasePtr->CanActivate != B.AbilityCasePtr->CanActivate)
				{
					return A.AbilityCasePtr->CanActivate > B.AbilityCasePtr->CanActivate;
				}
				return A.CategoryPriority > B.CategoryPriority;
		});
	}
	
}

#pragma endregion

#pragma region ////////////////////////////Ability Case 相关
FGameplayAbilitySpecHandle UExAbilitySystemComponent::GiveAbilityByCase(const FExAbilityCase& AbilityCase, bool ActivateOnce)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		ABILITY_LOG(Error, TEXT("%s error, ASC has no owner"), *FString(__FUNCTION__));
		return FGameplayAbilitySpecHandle();
	}

	ENetRole LocalRole = Owner->GetLocalRole();
	if (LocalRole == ENetRole::ROLE_Authority)
	{
		return GiveAbilityByCaseInternal(AbilityCase, ActivateOnce);
	}
	else if (LocalRole == ENetRole::ROLE_AutonomousProxy)
	{
		ServerGiveAbilityByCase(AbilityCase, ActivateOnce);
		ABILITY_LOG(Log, TEXT("%s in ROLE_AutonomousProxy, result will be invalid"), *FString(__FUNCTION__));
	}
	else
	{
		ABILITY_LOG(Error, TEXT("%s cannot be used in role: %d"), *FString(__FUNCTION__), LocalRole);
	}
	return FGameplayAbilitySpecHandle();
}

FGameplayAbilitySpec* UExAbilitySystemComponent::FindAbilitySpecFromCase(const FExAbilityCase& AbilityCase)
{
	if (AbilityCase.IsValid() == false)
	{
		return nullptr;
	}

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability == AbilityCase.AbilityClass->GetDefaultObject())
		{
			return &Spec;
		}
	}

	return nullptr;
}

FGameplayAbilitySpecHandle UExAbilitySystemComponent::GiveAbilityByCaseInternal(const FExAbilityCase& AbilityCase, bool ActivateOnce)
{
	if (AbilityCase.IsValid() == false)
	{
		ABILITY_LOG(Error, TEXT("UExAbilitySystemComponent::GiveAbilityByCaseInternal error, AbilityCase Invalid"));
		return FGameplayAbilitySpecHandle();
	}

	TObjectPtr<UObject> SourceObject = AbilityCase.SourceObject;
	if (!SourceObject)
	{
		SourceObject = this;
	}

	bool ActivateWhenGive = ActivateOnce ? true : AbilityCase.ActivateWhenGiven;

	//理论上每个Ability只Give一次
	if (FindAbilitySpecFromClass(AbilityCase.AbilityClass))
	{
		ABILITY_LOG(Warning, TEXT("%s , add duplicated class: %s, provider:%s"), *FString(__FUNCTION__), *GetNameSafe(AbilityCase.AbilityClass), *GetNameSafe(SourceObject));
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpec Spec(AbilityCase.AbilityClass, AbilityCase.AbilityLevel, INDEX_NONE, (SourceObject));
	const FGameplayAbilitySpecHandle& SpecHandle = GiveAbility(Spec);

	if (ActivateWhenGive)
	{
		TryActivateAbility(SpecHandle);
	}

	return SpecHandle;
}

bool UExAbilitySystemComponent::ServerGiveAbilityByCase_Validate(const FExAbilityCase& AbilityCase, bool ActivateOnce)
{
	return true;
}

void UExAbilitySystemComponent::ServerGiveAbilityByCase_Implementation(const FExAbilityCase& AbilityCase, bool ActivateOnce)
{
	GiveAbilityByCaseInternal(AbilityCase, ActivateOnce);
}


void UExAbilitySystemComponent::TryActivateAbilityOnceWithEventData_Implementation(const FExAbilityCase& AbilityCase, const FGameplayEventData& TriggerEventData, UObject* SourceObj)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromCase(AbilityCase);
	if (Spec && Spec->Handle.IsValid())
	{
		InternalTryActivateAbility(Spec->Handle, FPredictionKey(), nullptr, nullptr, &TriggerEventData);
	}
	else
	{
		EXABILITY_LOG(Error, TEXT("UExAbilitySystemComponent::TryActivateAbilityOnceWithEventData error, Spec invalid"));
	}
}

bool UExAbilitySystemComponent::TryActivateAbilityOnceWithEventData_Validate(const FExAbilityCase& AbilityCase, const FGameplayEventData& TriggerEventData, UObject* SourceObj)
{
	return true;
}

void UExAbilitySystemComponent::TryActivateAbilityByCase(const FExAbilityCase& AbilityCase)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromCase(AbilityCase);
	if (Spec && Spec->Handle.IsValid())
	{
		TryActivateAbility(Spec->Handle);
	}
	else
	{
		EXABILITY_LOG(Error, TEXT("UExAbilitySystemComponent::TryActivateAbilityByCase error, Handle for case is invalid"));
		return;
	}
}

void UExAbilitySystemComponent::RebuildAbilityCaseMap()
{
	AbilityCaseMap.Reset();
	for (FCollectedAbilityInfo& AbilityInfo : CollectedAbilityInfoList)
	{
		for (FExAbilityCase& AbilityCase : AbilityInfo.AbilityCases)
		{
			if (AbilityCase.IsValid())
			{
				if (!AbilityCaseMap.Contains(AbilityCase.AbilityClass))
				{
					AbilityCaseMap.Add(AbilityCase.AbilityClass, &AbilityCase);
				}
				else
				{
					EXABILITY_LOG(Error, TEXT("%s error, Duplicate abilityCase[%s] in %s with others"), *FString(__FUNCTION__), 
						*GetNameSafe(AbilityCase.AbilityClass), *AbilityInfo.ProviderInfo.ToString());
				}
			}
		}
	}
}

FExAbilityCase* UExAbilitySystemComponent::FindAbilityCaseByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	return nullptr;
}

#pragma endregion