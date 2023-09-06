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


UCLASS(Blueprintable, ClassGroup = AbilitySystem, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYABILITIES_API UExAbilitySystemComponent : public UAbilitySystemComponent, public IExAbilityProvider
{
	GENERATED_BODY()

public:
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
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
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

};
