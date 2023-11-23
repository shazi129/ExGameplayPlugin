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
#include "PawnStateLibrary.h"
#include "ExGameplayAbilityLibrary.h"
#include "PawnStateSubsystem.h"
#include "FunctionLibraries/TimeHelperLibrary.h"
#include "Kismet/GameplayStatics.h"

UExAbilitySystemComponent::UExAbilitySystemComponent()
{

}

void UExAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	//初始化默认的Attribute
	InitDefaultAttributes();

	//初始化默认的Effect
	InitDefaultEffects();

	//激活失败回调
	BindToAbilityFailedDelegate();

	GameplayEffectAddedHandle = OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UExAbilitySystemComponent::OnGameplayEffectAdded);
	GameplayEffectRemovedHandle = OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UExAbilitySystemComponent::OnGameplayEffectRemoved);

	if (UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(GetOwner()))
	{
		PawnStateComponent->OnPawnStateEventDelegate().AddDynamic(this, &UExAbilitySystemComponent::OnPawnStateEvent);
	}

	//注册本身的技能
	RegisterAbilityProvider(this);
}

void UExAbilitySystemComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UnbindAbilityFailedDelegate();
	ClearAttribuiteChangedDelegate();
	ClearAllProvider();

	if (UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(GetOwner()))
	{
		PawnStateComponent->OnPawnStateEventDelegate().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);

	OnActiveGameplayEffectAddedDelegateToSelf.Remove(GameplayEffectAddedHandle);
	OnAnyGameplayEffectRemovedDelegate().Remove(GameplayEffectRemovedHandle);
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

bool UExAbilitySystemComponent::CanActivateAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	//
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityClass);
	if (!Spec)
	{
		return false;
	}

	if (GetAbilityCooldown(AbilityClass) > 0.0f)
	{
		return false;
	}

	UGameplayAbility* AbilityCDO = Cast<UGameplayAbility>(AbilityClass->GetDefaultObject());
	if (!AbilityCDO)
	{
		return false;
	}

	if (!AbilityCDO->CanActivateAbility(Spec->Handle, AbilityActorInfo.Get()))
	{
		return false;
	}

	return true;
}

bool UExAbilitySystemComponent::HasGameplayTag(FGameplayTag TagToCheck) const
{
	int32 Count = GetTagCount(TagToCheck);
	if (Count > 0)
	{
		return true;
	}
	return false;
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

void UExAbilitySystemComponent::NotifyAbilityCommit(UGameplayAbility* Ability)
{
	Super::NotifyAbilityCommit(Ability);
}

void UExAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);
}

void UExAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

#pragma endregion

#pragma region //////////////////////////// Provider 相关

void UExAbilitySystemComponent::CollectAbilitCases(TArray<FExAbilityCase>& Abilities) const
{
	if (!DefaultAbilitySet.IsNull())
	{
		if (UExAbilityCaseSetAsset* AbilityCaseSetAsset = DefaultAbilitySet.LoadSynchronous())
		{
			for (const FExAbilityCase& AbilityCase : AbilityCaseSetAsset->AbilityCaseList)
			{
				Abilities.Add(AbilityCase);
			}
		}
		
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

void UExAbilitySystemComponent::ServerActivateAbilityByClass_Implementation(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (CanActivateAbility(AbilityClass))
	{
		TryActivateAbilityByClass(AbilityClass);
	}
}

#pragma endregion

#pragma region 
void UExAbilitySystemComponent::ServerActivateAbilityByCategory_Implementation(const FGameplayTag& CategoryTag)
{
	ActivateAbilityByCategory(CategoryTag);
}

void UExAbilitySystemComponent::CancelAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass);
	if (AbilitySpec)
	{
		CancelAbilitySpec(*AbilitySpec, nullptr);
	}
}

void UExAbilitySystemComponent::ServerCancelAbilityByClass_Implementation(TSubclassOf<UGameplayAbility> AbilityClass)
{
	CancelAbilityByClass(AbilityClass);
}

/////////////////////////////// 技能分类相关
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

	EXABILITY_LOG(Log, TEXT("%s Category %s Num:%d"), *FString(__FUNCTION__), *CategoryTag.ToString(), CategoryIndexList.Num());
	for (FAbilityCategoryIndex& CategoryIndex : CategoryIndexList)
	{
		if (CategoryIndex.AbilityCasePtr && CategoryIndex.AbilityCasePtr->IsValid() && CategoryIndex.AbilityCasePtr->CanActivate)
		{
			if (!CanActivateAbility(CategoryIndex.AbilityCasePtr->AbilityClass))
			{
				EXABILITY_LOG(Log, TEXT("%s check %s failed"), *FString(__FUNCTION__), *GetNameSafe(CategoryIndex.AbilityCasePtr->AbilityClass));
				continue;
			}

			//如果没give， give一次
			FGameplayAbilitySpec* Spec = FindAbilitySpecFromCase(*CategoryIndex.AbilityCasePtr);
			if (!Spec)
			{
				GiveAbilityByCase(*CategoryIndex.AbilityCasePtr, true);
				continue;
			}
			
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
		bool Result = TryActivateAbility(Spec->Handle);
		EXABILITY_LOG(Log, TEXT("%s %s result %d"), *FString(__FUNCTION__), *GetNameSafe(AbilityCase.AbilityClass), Result);
	}
	else
	{
		EXABILITY_LOG(Error, TEXT("%s error, Handle for case is invalid, %s"), *FString(__FUNCTION__), *GetNameSafe(AbilityCase.AbilityClass));
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


#pragma region 

//////////////////////////////// Attribute 相关

UOnAttributeValueChangeDelegateInfo* UExAbilitySystemComponent::GetAttribuiteChangedDelegate(FGameplayAttribute Attribute)
{
	if (!Attribute.IsValid())
	{
		EXABILITY_LOG(Error, TEXT("%s error, Param Attribute is invalid"), *FString(__FUNCTION__))
		return nullptr;
	}

	//如果没有定义回调，就创建一个，此时Attribute可能没真正创建(例如在beginPlay前调用此函数时，就需要在创建Attribute时再绑一遍, 参看CreateDefaultAttributes)
	UOnAttributeValueChangeDelegateInfo* DelegateInfo = nullptr;
	if (!AttributeDelegateMap.Contains(Attribute))
	{
		DelegateInfo = NewObject<UOnAttributeValueChangeDelegateInfo>(this, UOnAttributeValueChangeDelegateInfo::StaticClass());
		AttributeDelegateMap.Add(Attribute, DelegateInfo);
	}
	else
	{
		DelegateInfo = AttributeDelegateMap[Attribute];
	}

	//如果没有绑定, 绑定
	if (!DelegateInfo->BindHandle.IsValid())
	{
		const UAttributeSet* AttributeSet = GetAttributeSet(Attribute.GetAttributeSetClass());
		if (AttributeSet)
		{
			DelegateInfo->BindHandle = GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UExAbilitySystemComponent::OnAttributeChanged);
		}
		else
		{
			EXABILITY_LOG(Warning, TEXT("%s, Attribute[%s] does not exist"), *FString(__FUNCTION__), *Attribute.GetName());
		}
	}
	return DelegateInfo;
}

FGameplayAttributeData UExAbilitySystemComponent::GetAttributeData(FGameplayAttribute Attribute)
{
	const UAttributeSet* Attributeset =  GetAttributeSet(Attribute.GetAttributeSetClass());
	if (Attributeset)
	{
		FStructProperty* Property = FindFieldChecked<FStructProperty>(Attribute.GetAttributeSetClass(), FName(Attribute.AttributeName));
		if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
		{
			const FGameplayAttributeData* DataPtr = Property->ContainerPtrToValuePtr<FGameplayAttributeData>(Attributeset);
			if (DataPtr)
			{
				return *DataPtr;
			}
		}
					
	}
	return FGameplayAttributeData();
}

void UExAbilitySystemComponent::SetAttributeValue(FGameplayAttribute Attribute, float Value)
{
	ENetRole LocalRole = GetOwner()->GetLocalRole();
	if (LocalRole == ENetRole::ROLE_Authority)
	{
		ABILITY_LOG(Log, TEXT("%s %s %f"), *FString(__FUNCTION__), *Attribute.AttributeName, Value);
		SetNumericAttributeBase(Attribute, Value);
	}
	else if (LocalRole == ENetRole::ROLE_AutonomousProxy)
	{
		ServerSetAttributeValue(Attribute, Value);
	}
	else
	{
		ABILITY_LOG(Log, TEXT("%s error, Cannot set attribute in simulator"), *FString(__FUNCTION__), LocalRole);
	}
}

void UExAbilitySystemComponent::ApplyModifyToAttribute(FGameplayAttribute Attribute, TEnumAsByte<EGameplayModOp::Type> ModifierOp, float ModifierMagnitude)
{
	ENetRole LocalRole = GetOwner()->GetLocalRole();
	if (LocalRole == ENetRole::ROLE_Authority)
	{
		ABILITY_LOG(Log, TEXT("%s %s %d, %f"), *FString(__FUNCTION__), *Attribute.AttributeName, ModifierOp, ModifierMagnitude);
		Super::ApplyModToAttribute(Attribute, ModifierOp, ModifierMagnitude);
	}
	else if (LocalRole == ENetRole::ROLE_AutonomousProxy)
	{
		ServerApplyModifyToAttribute(Attribute, ModifierOp, ModifierMagnitude);
	}
	else
	{
		ABILITY_LOG(Log, TEXT("%s error, Cannot set attribute in simulator"), *FString(__FUNCTION__), LocalRole);
	}
}

bool UExAbilitySystemComponent::ServerApplyModifyToAttribute_Validate(const FGameplayAttribute& Attribute, EGameplayModOp::Type ModifierOp, float ModifierMagnitude)
{
	return true;
}

void UExAbilitySystemComponent::ServerApplyModifyToAttribute_Implementation(const FGameplayAttribute& Attribute, EGameplayModOp::Type ModifierOp, float ModifierMagnitude)
{
	Super::ApplyModToAttribute(Attribute, ModifierOp, ModifierMagnitude);
}

void UExAbilitySystemComponent::ResetDefaultAttributes()
{
	ENetRole LocalRole = GetOwner()->GetLocalRole();
	ABILITY_LOG(Log, TEXT("%s in %d"), *FString(__FUNCTION__), LocalRole);

	if (LocalRole == ENetRole::ROLE_Authority)
	{
		InternalResetDefaultAttributes();
	}
	else if (LocalRole == ENetRole::ROLE_AutonomousProxy)
	{
		ServerResetDefaultAttributes();
	}
	else
	{
		ABILITY_LOG(Log, TEXT("%s error, Cannot set attribute in simulator"), *FString(__FUNCTION__), LocalRole);
	}
}

bool UExAbilitySystemComponent::ServerResetDefaultAttributes_Validate()
{
	return true;
}

void UExAbilitySystemComponent::ServerResetDefaultAttributes_Implementation()
{
	InternalResetDefaultAttributes();
}

void UExAbilitySystemComponent::InternalResetDefaultAttributes()
{
	//初始化Attribute
	UExGameplayAbilityLibrary::ApplyGameplayEffectClass(this, AttributeConfig.InitAttributesEffectClass);
}

bool UExAbilitySystemComponent::ServerSetAttributeValue_Validate(FGameplayAttribute Attribute, float Value)
{
	return true;
}
void UExAbilitySystemComponent::ServerSetAttributeValue_Implementation(FGameplayAttribute Attribute, float Value)
{
	SetNumericAttributeBase(Attribute, Value);
}

void UExAbilitySystemComponent::InitDefaultAttributes()
{
	if (AttributeConfigAsset.IsNull())
	{
		return;
	}

	UExAttributeConfigAsset* Assets = AttributeConfigAsset.LoadSynchronous();
	if (!Assets)
	{
		return;
	}
	AttributeConfig = Assets->AttributeConfig;

	CreateDefaultAttributes();
	InternalResetDefaultAttributes();

	//确保AttributeEvent 有回调
	for (auto& AttributeChangeEventItem : AttributeConfig.AttributeChangeEventMap)
	{
		GetAttribuiteChangedDelegate(AttributeChangeEventItem.Key);
	}
}

void UExAbilitySystemComponent::CreateDefaultAttributes()
{
	//创建Attribute
	for (auto& AttributesClass : AttributeConfig.DefaultAttributesClassList)
	{
		//AttributeSet只能挂在Actor上
		UAttributeSet* AttributeSet = NewObject<UAttributeSet>(this->GetOwner(), AttributesClass);
		if (AttributeSet)
		{
			AddSpawnedAttribute(AttributeSet);
			AttributeSetObjectMap.Add(AttributeSet->GetClass(), AttributeSet);
		}
	}

	//如果回调没绑上，现在再绑一次
	for (auto& DelegateInfoItem : AttributeDelegateMap)
	{
		UOnAttributeValueChangeDelegateInfo* DelegateInfo = DelegateInfoItem.Value;
		if (DelegateInfo && !DelegateInfo->BindHandle.IsValid())
		{
			FOnGameplayAttributeValueChange& AttributeValueChangeDelegate = GetGameplayAttributeValueChangeDelegate(DelegateInfoItem.Key);
			DelegateInfo->BindHandle = AttributeValueChangeDelegate.AddUObject(this, &UExAbilitySystemComponent::OnAttributeChanged);

			//触发一遍
			float BaseValue = GetAttributeData(DelegateInfoItem.Key).GetBaseValue();
			FOnAttributeChangeData ChangeData;
			ChangeData.NewValue = BaseValue;
			ChangeData.OldValue = BaseValue;
			OnAttributeChanged(ChangeData);
		}
	}
}

void UExAbilitySystemComponent::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	FExOnAttributeChangeData ChangeData(Data);

	if (AttributeDelegateMap.Contains(Data.Attribute))
	{
		UOnAttributeValueChangeDelegateInfo* DelegateInfo = AttributeDelegateMap[Data.Attribute];
		//数据没发生过变化
		if (DelegateInfo->OldValue == Data.NewValue && Data.NewValue == Data.OldValue)
		{
			return;
		}

		//广播
		if (DelegateInfo->OldValue >= 0)
		{
			ChangeData.OldValue = DelegateInfo->OldValue;
		}
		DelegateInfo->Delegate.Broadcast(FExOnAttributeChangeData(Data));
		DelegateInfo->OldValue = Data.NewValue;
	}
	else
	{
		EXABILITY_LOG(Error, TEXT("%s %s not bind to delegate"), *FString(__FUNCTION__), *Data.Attribute.AttributeName);
	}

	ENetRole LocalRole = GetOwner()->GetLocalRole();
	//if (LocalRole == ENetRole::ROLE_AutonomousProxy)
	{
		EXABILITY_LOG(Log, TEXT("%s %s,  Role:%d OldValue:%f, NewValue:%f"), *FString(__FUNCTION__), *Data.Attribute.AttributeName, LocalRole, ChangeData.OldValue, ChangeData.NewValue);
	}

	//处理Event
	if (FAttributeChangeEventList* ChangeEventListPtr = AttributeConfig.AttributeChangeEventMap.Find(Data.Attribute))
	{
		for (FInstancedStruct& EventItem : ChangeEventListPtr->EventList)
		{
			const UScriptStruct* EventType = EventItem.GetScriptStruct();
			if (!EventType->IsChildOf(FAttributeChangeEvent::StaticStruct()))
			{
				continue;
			}
			FAttributeChangeEvent& Event = EventItem.GetMutable<FAttributeChangeEvent>();
			Event.Handle(this, Data);
		}
	}
}
void UExAbilitySystemComponent::ClearAttribuiteChangedDelegate()
{
	for (auto& DelegateInfoItem : AttributeDelegateMap)
	{
		DelegateInfoItem.Value->Delegate.RemoveAll(this);
	}
	AttributeDelegateMap.Empty();
	AttributeConfig.Reset();
}

#pragma endregion


#pragma region //////////////////////////////// Effect 相关

FActiveGameplayEffectHandle UExAbilitySystemComponent::ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey)
{
	UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(GetOwner());
	if (PawnStateComponent && GameplayEffect.Def)
	{
		FString ErrMsg;
		bool Result = PawnStateComponent->CanEnterPawnStates(GameplayEffect.Def->InheritableOwnedTagsContainer.CombinedTags, ErrMsg);
		if (!Result)
		{
			EXABILITY_LOG(Log, TEXT("%s, %s apply canceled: %s"), *FString(__FUNCTION__), *GetNameSafe(GameplayEffect.Def), *ErrMsg);
			return FActiveGameplayEffectHandle();
		}
	}
	return Super::ApplyGameplayEffectSpecToSelf(GameplayEffect, PredictionKey);
}

void UExAbilitySystemComponent::OnGameplayEffectAdded(UAbilitySystemComponent* Owner, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	//只在主控端和服务端运行
	ENetRole Role = GetOwner()->GetLocalRole();
	if (Role != ENetRole::ROLE_Authority && Role != ENetRole::ROLE_AutonomousProxy)
	{
		return;
	}

	//如果这个Effect是技能触发的
	const UGameplayAbility* ContextAbility = Spec.GetEffectContext().GetAbility();
	if (ContextAbility)
	{
		//如果是Cooldown的Effect, 记录下cooldown时间
		if (Spec.Duration > 0 && ContextAbility->GetCooldownGameplayEffect()->GetClass() == Spec.Def.GetClass())
		{
			ApplyAbilityCooldown(ContextAbility->GetClass(), Spec.Duration);
		}
	}

	UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(GetOwner());
	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);
	if (PawnStateComponent && PawnStateSubsystem && Spec.Def != nullptr)
	{
		for (auto& EffectTag : Spec.Def->InheritableOwnedTagsContainer.CombinedTags)
		{
			//if (PawnStateSubsystem->GetPawnStateAsset(EffectTag) != nullptr)
			{ 
				int StateID = PawnStateComponent->InternalEnterPawnState(EffectTag, const_cast<UGameplayEffect*>(Spec.Def.Get()), nullptr);
				EffectStateIDMap.FindOrAdd(Handle).Add(StateID);
			}
		}
	}
}

void UExAbilitySystemComponent::OnGameplayEffectRemoved(const FActiveGameplayEffect& Effect)
{
	//只在主控端和服务端运行
	ENetRole Role = GetOwner()->GetLocalRole();
	if (Role != ENetRole::ROLE_Authority && Role != ENetRole::ROLE_AutonomousProxy)
	{
		return;
	}

	UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(GetOwner());
	if (PawnStateComponent && Effect.Spec.Def != nullptr)
	{
		for (auto& EffectTag : Effect.Spec.Def->InheritableOwnedTagsContainer.CombinedTags)
		{
			auto EffectStates = EffectStateIDMap.Find(Effect.Handle);
			if (EffectStates)
			{
				for (int32 StateID : *EffectStates)
				{
					PawnStateComponent->InternalLeavePawnState(StateID, nullptr);
				}
			}
		}
	}
}

void UExAbilitySystemComponent::InitDefaultEffects()
{
	for (auto& EffectClass : DefaultEffectClassList)
	{
		UExGameplayAbilityLibrary::ApplyGameplayEffectClass(this, EffectClass);
	}
}

#pragma endregion

#pragma region //////////////////////////////// 技能失败回调
void UExAbilitySystemComponent::BindToAbilityFailedDelegate()  
{  
	AbilityFailedHandle = AbilityFailedCallbacks.AddUObject(this, &UExAbilitySystemComponent::OnAbilityFailed);  
}

void UExAbilitySystemComponent::UnbindAbilityFailedDelegate()
{
	AbilityFailedCallbacks.Remove(AbilityFailedHandle);
}

void UExAbilitySystemComponent::OnAbilityFailed(const UGameplayAbility* FailedAbility, const FGameplayTagContainer& FailTags)
{
	auto ExAbility = Cast<UExGameplayAbility>(FailedAbility);
	if(ExAbility)
	{
		ExAbility->OnAbilityFailed(FailTags);
	}
}
#pragma endregion 


#pragma region /////////////////////////////////一些运行时状态
const UExGameplayAbility* UExAbilitySystemComponent::GetCurrentApplyCostAbility()
{
	return CurrentApplyCostAbility;
}

void UExAbilitySystemComponent::SetCurrentApplyCostAbility(const UExGameplayAbility* Ability)
{
	CurrentApplyCostAbility = Ability;
}
#pragma endregion 

void UExAbilitySystemComponent::OnPawnStateEvent(EPawnStateEventType EventType, const FPawnStateInstance& Instance)
{
	UGameplayEffect* SourceEffect = Cast<UGameplayEffect>(Instance.SourceObject.Get());
	if (!SourceEffect)
	{
		UpdateTagMap(Instance.PawnStateTag, EventType == EPawnStateEventType::E_Enter? 1 : -1);
	}

	//当state是被其他state排斥出去的情况
	if (EventType == EPawnStateEventType::E_Leave && Instance.Instigator != nullptr)
	{
		EXABILITY_LOG(Log, TEXT("%s, %s was cancelled"), *FString(__FUNCTION__), *Instance.ToString());

		//退出技能
		if (UExGameplayAbility* ExAbility = Cast<UExGameplayAbility>(Instance.SourceObject.Get()))
		{
			ExAbility->ClearPawnState();
			FGameplayAbilitySpecHandle Handle = ExAbility->GetCurrentAbilitySpecHandle();
			CancelAbilityHandle(Handle);
		}
		//退出Effect
		else if (UGameplayEffect* Effect = Cast<UGameplayEffect>(Instance.SourceObject.Get()))
		{
			RemoveActiveGameplayEffectBySourceEffect(Effect->GetClass(), this);
		}
	}
}

void UExAbilitySystemComponent::ApplyAbilityCooldown(TSubclassOf<UGameplayAbility> AbilityClass, float Seconds)
{
	if (Seconds > 0.0f)
	{
		double CurrentTime = UGameplayStatics::GetTimeSeconds(this);
		AbilityCooldownEndMap.FindOrAdd(AbilityClass) = CurrentTime + Seconds;
		AbilityCooldownDelegate.Broadcast(AbilityClass, Seconds);
	}
}

double UExAbilitySystemComponent::GetAbilityCooldown(TSubclassOf<UGameplayAbility> AbilityClass)
{
	double* EndTimePtr = AbilityCooldownEndMap.Find(AbilityClass);
	if (!EndTimePtr)
	{
		return 0.0f;
	}

	double CurrentTime = UGameplayStatics::GetTimeSeconds(this);
	double Cooldown = *EndTimePtr - CurrentTime;
	if (Cooldown <= 0.0f)
	{
		AbilityCooldownEndMap.Remove(AbilityClass);
		return 0.0f;
	}
	return Cooldown;
}

void UExAbilitySystemComponent::ClearAbilityCooldown()
{
	AbilityCooldownEndMap.Empty();
}
