// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/Actor.h"
#include "ExGameplayAbility.h"
#include "ExAttributeSet.h"
#include "ExAbilityProvider.h"
#include "ExAttributeTypes.h"
#include "ExAbilitySystemComponent.generated.h"


//Category索引
USTRUCT()
struct EXGAMEPLAYABILITIES_API FAbilityCategoryIndex
{
	GENERATED_BODY()
	
	FExAbilityCase* AbilityCasePtr;

	int CategoryPriority;
};

//初始化Attribute的方式
UENUM(BlueprintType)
enum class EInitAttributeMethod : uint8
{
	//通过GameplayEffect来初始化
	E_GameplayEffect		UMETA(DisplayName = "GameplayEffect"),

	//通过DataTable来初始化
	E_DataTable			UMETA(DisplayName = "DataTable"),
};

UCLASS(Blueprintable, ClassGroup = AbilitySystem, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYABILITIES_API UExAbilitySystemComponent : public UAbilitySystemComponent, public IExAbilityProvider
{
	GENERATED_BODY()

public:

	UExAbilitySystemComponent();

	//override UAbilitySystemComponent
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:

	UFUNCTION()
		void OnPawnControllerChanged(APawn* Pawn, AController* NewController);

	UFUNCTION(BlueprintCallable, Category = ExAbility)
		void ClearAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation, Category = ExAbility)
		void ServerClearAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
		void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
		void AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
		void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Client, Reliable)
		void ClientPlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None, float StartTimeSeconds = 0.0f);

	UFUNCTION(BlueprintCallable, Client, Reliable)
		void ClientStopMontage();

	virtual bool CanActivateAbility(TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = ExAbility)
		bool HasGameplayTag(FGameplayTag TagToCheck) const;
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Abilites")
		TArray<FExAbilityCase> DefaultAbilities;

private:
	
	void ClearAbilityByClassInternal(TSubclassOf<UGameplayAbility> AbilityClass);

#pragma region //////////////////////////// Override AbilitySystemComponent
public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void NotifyAbilityCommit(UGameplayAbility* Ability) override;
	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
#pragma endregion

#pragma region //////////////////////////// Provider 相关
public:
	//本身也是个Provider
	virtual void CollectAbilitCases(TArray<FExAbilityCase>& Abilities) const override;

	//Provider的注册和反注册
	UFUNCTION(BlueprintCallable, Category="ExAbilitySystemComponent")
	virtual void RegisterAbilityProvider(TScriptInterface<IExAbilityProvider> Provider);

	UFUNCTION(BlueprintCallable, Category = "ExAbilitySystemComponent")
	virtual void UnregisterAbilityProvider(TScriptInterface<IExAbilityProvider> Provider);

protected:
	virtual void ClearAllProvider();
	virtual void ClearCollectedAbilities(FCollectedAbilityInfo& CollectedAbilityInfo, bool NeedRebuildCategory=false);

private:
	void ReCollectedAbilityInfo();

private:
	TArray<FCollectedAbilityInfo> CollectedAbilityInfoList;
#pragma endregion

#pragma region ////////////////////////////技能分类相关
public:

	UFUNCTION(BlueprintCallable, Server, reliable, Category = ExAbility)
	void ServerActivateAbilityByCategory(const FGameplayTag& CategoryTag);

	UFUNCTION(BlueprintCallable, Category = ExAbility)
	void ActivateAbilityByCategory(const FGameplayTag& CategoryTag);

private:
	void RebuildAbilityCategory();
	TMap<FGameplayTag, TArray<FAbilityCategoryIndex>> AbilityCategoryMap;
	
#pragma endregion

#pragma region ////////////////////////////Ability Case 相关
public:
	UFUNCTION(BlueprintCallable, Category = ExAbility)
	FGameplayAbilitySpecHandle GiveAbilityByCase(const FExAbilityCase& AbilityCase, bool ActivateOnce=false);

	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation, Category = ExAbility)
	void ServerGiveAbilityByCase(const FExAbilityCase& AbilityCase, bool ActivateOnce = false);

	FGameplayAbilitySpec* FindAbilitySpecFromCase(const FExAbilityCase& AbilityCase);

	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation)
	virtual void TryActivateAbilityOnceWithEventData(const FExAbilityCase& AbilityCase, const FGameplayEventData& TriggerEventData, UObject* SourceObj);

	UFUNCTION(BlueprintCallable, Category = ExAbility)
	void TryActivateAbilityByCase(const FExAbilityCase& AbilityCase);

	FExAbilityCase* FindAbilityCaseByClass(TSubclassOf<UGameplayAbility> AbilityClass);
	
private:
	FGameplayAbilitySpecHandle GiveAbilityByCaseInternal(const FExAbilityCase& AbilityCase, bool ActivateOnce=false);
	void RebuildAbilityCaseMap();
private:
	TMap<TSubclassOf<UGameplayAbility>, FExAbilityCase*> AbilityCaseMap;
#pragma endregion


#pragma region //////////////////////////////// Attribute 相关

public:
	UFUNCTION(BlueprintPure)
	UOnAttributeValueChangeDelegateInfo* GetAttribuiteChangedDelegate(FGameplayAttribute Attribute);

	UFUNCTION(BlueprintPure)
	FGameplayAttributeData GetAttributeData(FGameplayAttribute Attribute);

	UFUNCTION(BlueprintCallable)
	void SetAttributeValue(FGameplayAttribute Attribute, float Value);

	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation, Category = ExAbility)
	void ServerSetAttributeValue(FGameplayAttribute Attribute, float Value);

	UFUNCTION(BlueprintCallable)
	void ApplyModifyToAttribute(const FGameplayAttribute& Attribute, TEnumAsByte<EGameplayModOp::Type> ModifierOp, float ModifierMagnitude);

	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation, Category = ExAbility)
	void ServerApplyModifyToAttribute(const FGameplayAttribute& Attribute, EGameplayModOp::Type ModifierOp, float ModifierMagnitude);

	//重置Attribtes到默认值
	UFUNCTION(BlueprintCallable)
	void ResetDefaultAttributes();

	//Server端的重置
	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation, Category = ExAbility)
	void ServerResetDefaultAttributes();

protected:
	//默认的一些属性
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	TSoftObjectPtr<UExAttributeConfigAsset> AttributeConfigAsset;

protected:
	void InitDefaultAttributes();
	void CreateDefaultAttributes();
	virtual void InternalResetDefaultAttributes();
	virtual void OnAttributeChanged(const FOnAttributeChangeData& Data);
	virtual void ClearAttribuiteChangedDelegate();
protected:

	UPROPERTY()
	FExAttributeConfig AttributeConfig;

	TMap<TSubclassOf<UAttributeSet>, TObjectPtr<UAttributeSet>> AttributeSetObjectMap;

	UPROPERTY(Transient)
	TMap<FGameplayAttribute, UOnAttributeValueChangeDelegateInfo*> AttributeDelegateMap;

#pragma endregion

#pragma region //////////////////////////////// Effect 相关

protected:

	UFUNCTION()
	virtual void OnGameplayEffectAdded(UAbilitySystemComponent* Owner, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);

	UFUNCTION()
	virtual void OnGameplayEffectRemoved(const FActiveGameplayEffect& Effect);

	void InitDefaultEffects();

	//默认的一些Effect，例如血量回复，默认的buff之类
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffectClassList;

private:
	FDelegateHandle GameplayEffectAddedHandle;
	FDelegateHandle GameplayEffectRemovedHandle;

	//当前生效的Effect， key： effect handle, value: pawnstate id
	TMap<FActiveGameplayEffectHandle, TArray<int32>> EffectStateIDMap;

#pragma endregion

#pragma region //////////////////////////////// 技能失败回调
public:
	UFUNCTION(Category = "Ability")  
	void OnAbilityFailed(const UGameplayAbility* FailedAbility, const FGameplayTagContainer& FailTags);

	
private:
	UFUNCTION(BlueprintCallable, Category = "Ability")  
	void BindToAbilityFailedDelegate();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void UnbindAbilityFailedDelegate();
	
	FDelegateHandle AbilityFailedHandle;
#pragma endregion

#pragma region /////////////////////////////////一些运行时状态
public:
	UFUNCTION(BlueprintPure)
	const UExGameplayAbility* GetCurrentApplyCostAbility();

	void SetCurrentApplyCostAbility(const UExGameplayAbility* Ability);
protected:
	TObjectPtr<const UExGameplayAbility> CurrentApplyCostAbility;
#pragma endregion


public:
	UFUNCTION()
	void OnAbilityStateLeave(const FPawnStateInstance& PawnStateInstance);

private:
	TArray<FGameplayTag> AbilityStateLeaveTags;
};
