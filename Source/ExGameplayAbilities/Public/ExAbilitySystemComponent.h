// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/Actor.h"
#include "ExGameplayAbility.h"
#include "ExAttributeSet.h"
#include "ExAbilityProvider.h"
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

//Attribute变化类型
UENUM(BlueprintType)
enum class EAttributeChangeEventType : uint8
{
	//阈值类型
	E_Threshold		UMETA(DisplayName = "Threshold"),

	//增加类型
	E_Increase			UMETA(DisplayName = "Increase"),

	//减少类型
	E_Decrease			UMETA(DisplayName = "Decrease"),
};

//Attribut变化的一些事件
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeChangeEventItem
{
	GENERATED_BODY()

	//监听的事件类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttributeChangeEventType EventType = EAttributeChangeEventType::E_Threshold;

	//触发的GameplayEffect
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> EffectClass;

	//触发阈值
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EventType == EAttributeChangeEventType::E_Threshold", EditConditionHides))
	float ApplyThreshold;

	//移除阈值
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EventType == EAttributeChangeEventType::E_Threshold", EditConditionHides))
	float RemoveThreshold;

	//

	FActiveGameplayEffectHandle EffectHandle;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeChangeEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAttributeChangeEventItem> EventItemList;
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

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Abilites")
		TArray<FExAbilityCase> DefaultAbilities;

private:
	
	void ClearAbilityByClassInternal(TSubclassOf<UGameplayAbility> AbilityClass);

#pragma region //////////////////////////// Override AbilitySystemComponent
public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
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
	UFUNCTION(BlueprintCallable)
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

protected:

	//默认的一些属性
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	TArray<TSubclassOf<UAttributeSet>> DefaultAttributesClassList;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	EInitAttributeMethod InitAttributeMethod = EInitAttributeMethod::E_GameplayEffect;

	//初始化默认属性的Effect
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes", meta = (EditCondition = "InitAttributeMethod == EInitAttributeMethod::E_GameplayEffect", EditConditionHides))
	TSubclassOf<UGameplayEffect> InitAttributesEffectClass;

	//初始化默认属性的DataTable
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes", meta = (EditCondition = "InitAttributeMethod == EInitAttributeMethod::E_DataTable", EditConditionHides))
	UDataTable* InitAttributeDataTable;

	//Attribute变化的事件
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes|ChangeEvent")
	TMap<FGameplayAttribute, FAttributeChangeEvent> AttributeChangeEventMap;

protected:
	void InitDefaultAttributes();
	virtual void OnAttributeChanged(const FOnAttributeChangeData& Data);
	virtual void HandleAttributeChangeEvent(FAttributeChangeEventItem& EventItem, const FOnAttributeChangeData& Data);

protected:
	TMap<TSubclassOf<UAttributeSet>, TObjectPtr<UAttributeSet>> AttributeSetObjectMap;

	UPROPERTY(Transient)
	TMap<FGameplayAttribute, UOnAttributeValueChangeDelegateInfo*> AttributeDelegateMap;
#pragma endregion

#pragma region //////////////////////////////// Effect 相关

public:
	virtual FActiveGameplayEffectHandle ApplyGameplayEffectClass(TSubclassOf<UGameplayEffect> EffectClass);

protected:

	void InitDefaultEffects();

	//默认的一些Effect，例如血量回复，默认的buff之类
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffectClassList;

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

};
