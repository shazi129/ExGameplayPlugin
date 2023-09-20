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
#include "PawnStateComponent.h"
#include "PawnStateSubsystem.h"

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

	//注册本身的技能
	RegisterAbilityProvider(this);

	//激活失败回调
	BindToAbilityFailedDelegate();
}

void UExAbilitySystemComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UnbindAbilityFailedDelegate();
	
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

bool UExAbilitySystemComponent::CanActivateAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	UGameplayAbility* CDO = Cast<UGameplayAbility>(AbilityClass->GetDefaultObject());
	UPawnStateSubsystem* PawnStateSubsystem = UPawnStateSubsystem::GetSubsystem(this);

	//UPawnStateComponent* PawnStateComponent = Cast<UPawnStateComponent>(GetOwner()->GetComponentByClass(UPawnStateComponent::StaticClass()));
	//if (PawnStateComponent)
	//{
	//	for (const FGameplayTag& AbilityTag : CDO->AbilityTags)
	//	{
	//		if (PawnStateComponent->CanEnterPawnState(AbilityTag, ))
	//	}
	//}
	return true;
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

	EXABILITY_LOG(Log, TEXT("%s Category %s Num:%d"), *FString(__FUNCTION__), *CategoryTag.ToString(), CategoryIndexList.Num());
	for (FAbilityCategoryIndex& CategoryIndex : CategoryIndexList)
	{
		if (CategoryIndex.AbilityCasePtr && CategoryIndex.AbilityCasePtr->IsValid() && CategoryIndex.AbilityCasePtr->CanActivate)
		{
			if (!CanActivateAbility(CategoryIndex.AbilityCasePtr->AbilityClass))
			{
				EXABILITY_LOG(Log, TEXT("%s ignore ability %s"), *FString(__FUNCTION__), *GetNameSafe(CategoryIndex.AbilityCasePtr->AbilityClass));
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
		if (!TryActivateAbility(Spec->Handle))
		{
			EXABILITY_LOG(Error, TEXT("%s error, TryActivateAbility %s error"), *FString(__FUNCTION__), *GetNameSafe(AbilityCase.AbilityClass));
		}
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

	//先加进入
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
	ABILITY_LOG(Log, TEXT("%s in %d"), *FString(__FUNCTION__), LocalRole);

	if (LocalRole == ENetRole::ROLE_Authority)
	{
		SetNumericAttributeBase(Attribute, Value);
	}
	else if (LocalRole == ENetRole::ROLE_AutonomousProxy)
	{
		ServerSetAttributeValue(Attribute, Value);
	}
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
	//创建Attribute
	for (auto& AttributesClass : DefaultAttributesClassList)
	{
		//AttributeSet只能挂在Actor上
		UAttributeSet* AttributeSet = NewObject<UAttributeSet>(this->GetOwner(), AttributesClass);
		if (AttributeSet)
		{
			AddSpawnedAttribute(AttributeSet);
			AttributeSetObjectMap.Add(AttributeSet->GetClass(), AttributeSet);
		}
	}

	//如果回掉没绑上，现在再绑一次
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

	//初始化Attribute
	if (InitAttributeMethod == EInitAttributeMethod::E_GameplayEffect && InitAttributesEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(InitAttributesEffectClass, 1, MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle EffectHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!EffectHandle.IsValid())
			{
				EXABILITY_LOG(Warning, TEXT("%s ApplyGameplayEffectSpecToSelf return invalide handle"), *FString(__FUNCTION__));
			}
		}
		else
		{
			EXABILITY_LOG(Error, TEXT("%s error, MakeOutgoingSpec error"), *FString(__FUNCTION__));
		}
	}
	else if (InitAttributeMethod == EInitAttributeMethod::E_DataTable && InitAttributeDataTable)
	{
		static const FString Context = FString(TEXT("UAttribute::BindToMetaDataTable"));
		for (auto& AttributeSetItem : AttributeSetObjectMap)
		{
			FGameplayAttribute GameplayAttribute;
			FExOnAttributeChangeData ExOnAttributeChangeData;

			//改编自UAttributeSet::InitFromMetaDataTable， 增加通知
			for (TFieldIterator<FProperty> It(AttributeSetItem.Value->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
			{
				FProperty* Property = *It;
				if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
				{
					FString RowNameStr = FString::Printf(TEXT("%s.%s"), *Property->GetOwnerVariant().GetName(), *Property->GetName());
					FAttributeMetaData* MetaData = InitAttributeDataTable->FindRow<FAttributeMetaData>(FName(*RowNameStr), Context, false);
					if (MetaData)
					{
						FStructProperty* StructProperty = CastField<FStructProperty>(Property);
						check(StructProperty);
						FGameplayAttributeData* DataPtr = StructProperty->ContainerPtrToValuePtr<FGameplayAttributeData>(AttributeSetItem.Value);
						check(DataPtr);
						DataPtr->SetBaseValue(MetaData->BaseValue);
						DataPtr->SetCurrentValue(MetaData->BaseValue);

						GameplayAttribute.SetUProperty(Property);
						ExOnAttributeChangeData.Set(0, MetaData->BaseValue);
						if (AttributeDelegateMap.Contains(GameplayAttribute))
						{
							UOnAttributeValueChangeDelegateInfo* DelegateInfo = AttributeDelegateMap[GameplayAttribute];
							DelegateInfo->Delegate.Broadcast(ExOnAttributeChangeData);
						}
					}
				}
			}
		}
	}

	//确保AttributeEvent 有回调
	for (auto& AttributeChangeEventItem : AttributeChangeEventMap)
	{
		GetAttribuiteChangedDelegate(AttributeChangeEventItem.Key);
	}
}

void UExAbilitySystemComponent::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		EXABILITY_LOG(Log, TEXT("%s, OldValue:%f, NewValue:%f"), *FString(__FUNCTION__), Data.OldValue, Data.NewValue);
	}

	if (AttributeDelegateMap.Contains(Data.Attribute))
	{
		UOnAttributeValueChangeDelegateInfo* DelegateInfo = AttributeDelegateMap[Data.Attribute];
		DelegateInfo->Delegate.Broadcast(FExOnAttributeChangeData(Data));
	}

	if (FAttributeChangeEvent* ChangeEventPtr = AttributeChangeEventMap.Find(Data.Attribute))
	{
		for (FAttributeChangeEventItem& EventItem : ChangeEventPtr->EventItemList)
		{
			HandleAttributeChangeEvent(EventItem, Data);
		}
	}

	//TODO: 处理Attribute Event消息, 目前只做了减少的处理，例如体力小于0时触发，大于100时结束。反过来的还没做
	// 以tradeup为标志，true时表示以上升趋势触发，false时表示以下降趋势触发
	bool TradeUp = true;
	if (Data.OldValue > Data.NewValue) //值减少
	{
		if (FAttributeChangeEvent* ChangeEventPtr = AttributeChangeEventMap.Find(Data.Attribute))
		{
			for (FAttributeChangeEventItem& EventItem : ChangeEventPtr->EventItemList)
			{
				TradeUp = EventItem.ApplyThreshold - EventItem.RemoveThreshold > 0 ? true : false;
				if(TradeUp)
				{
					//相等的话，默认不开启
					if (EventItem.RemoveThreshold == EventItem.ApplyThreshold  //不需要开启remove
						|| Data.NewValue > EventItem.RemoveThreshold           //数值不符
						|| !EventItem.EffectHandle.WasSuccessfullyApplied() //没有apply过
						)
					{
						continue;
					}

					if (EventItem.EffectHandle.IsValid()) //instant effect不需要移除
						{
							RemoveActiveGameplayEffect(EventItem.EffectHandle);
						}
					EventItem.EffectHandle = FActiveGameplayEffectHandle();
				} else
				{
					if (EventItem.EffectClass == nullptr || Data.NewValue > EventItem.ApplyThreshold)
					{
						continue;
					}

					//当前有同样的Effect在跑，清除老的
					if (GetGameplayEffectCount(EventItem.EffectClass, this) > 0)
					{
						RemoveActiveGameplayEffectBySourceEffect(EventItem.EffectClass, this);
					}

					EventItem.EffectHandle = ApplyGameplayEffectClass(EventItem.EffectClass);
				}
			}
		}
	}
	else if (Data.OldValue < Data.NewValue)//值增加
	{
		if (FAttributeChangeEvent* ChangeEventPtr = AttributeChangeEventMap.Find(Data.Attribute))
		{
			for (FAttributeChangeEventItem& EventItem : ChangeEventPtr->EventItemList)
			{
				TradeUp = EventItem.ApplyThreshold - EventItem.RemoveThreshold > 0 ? true : false;
				if(TradeUp)
				{
					if (EventItem.EffectClass == nullptr || Data.NewValue < EventItem.ApplyThreshold)
					{
						continue;
					}

					//当前有同样的Effect在跑，清除老的
					if (GetGameplayEffectCount(EventItem.EffectClass, this) > 0)
					{
						RemoveActiveGameplayEffectBySourceEffect(EventItem.EffectClass, this);
					}

					EventItem.EffectHandle = ApplyGameplayEffectClass(EventItem.EffectClass);
				} else
				{
					//相等的话，默认不开启
					if (EventItem.RemoveThreshold == EventItem.ApplyThreshold  //不需要开启remove
						|| Data.NewValue < EventItem.RemoveThreshold           //数值不符
						|| !EventItem.EffectHandle.WasSuccessfullyApplied() //没有apply过
						)
					{
						continue;
					}

					if (EventItem.EffectHandle.IsValid()) //instant effect不需要移除
						{
						RemoveActiveGameplayEffect(EventItem.EffectHandle);
						}
					EventItem.EffectHandle = FActiveGameplayEffectHandle();
				}
			}
		}
	}
}

void UExAbilitySystemComponent::HandleAttributeChangeEvent(FAttributeChangeEventItem& EventItem, const FOnAttributeChangeData& Data)
{
	bool NeedApply = false;
	bool NeedRemove = false;


	if (EventItem.EventType == EAttributeChangeEventType::E_Decrease)
	{
		NeedApply = Data.NewValue < Data.OldValue;
	}
	else if (EventItem.EventType == EAttributeChangeEventType::E_Increase)
	{
		NeedApply = Data.NewValue > Data.OldValue;
	}
	else if (EventItem.EventType == EAttributeChangeEventType::E_Threshold)
	{
		float Ratio = EventItem.RemoveThreshold > EventItem.ApplyThreshold ? 1 : -1;

	}


	if (NeedApply)
	{
		EventItem.EffectHandle = ApplyGameplayEffectClass(EventItem.EffectClass);
	}
	else if (NeedRemove)
	{
		if (EventItem.EffectHandle.IsValid()) //instant effect不需要移除
		{
			RemoveActiveGameplayEffect(EventItem.EffectHandle);
		}
		EventItem.EffectHandle = FActiveGameplayEffectHandle();
	}
}

#pragma endregion


#pragma region //////////////////////////////// Effect 相关

FActiveGameplayEffectHandle UExAbilitySystemComponent::ApplyGameplayEffectClass(TSubclassOf<UGameplayEffect> EffectClass)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
	if (SpecHandle.IsValid())
	{
		return ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	return FActiveGameplayEffectHandle();
}

void UExAbilitySystemComponent::InitDefaultEffects()
{
	for (auto& EffectClass : DefaultEffectClassList)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle EffectHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!EffectHandle.IsValid())
			{
				EXABILITY_LOG(Warning, TEXT("%s ApplyGameplayEffectSpecToSelf return invalide handle"), *FString(__FUNCTION__));
			}
		}
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

