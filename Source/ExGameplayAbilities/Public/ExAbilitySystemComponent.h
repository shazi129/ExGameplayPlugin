// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/Actor.h"
#include "ExGameplayAbility.h"
#include "ExAttributeSet.h"
#include "ExAbilityProvider.h"
#include "ExAbilitySystemComponent.generated.h"

USTRUCT()
struct FExAttributeApplication
{
	GENERATED_BODY()

		// Ability set to grant
		UPROPERTY(EditAnywhere)
		TSubclassOf<UExAttributeSet> AttributeSetType;

	// Data table reference to initialize the attributes with, if any (can be left unset)
	UPROPERTY(EditAnywhere)
		UDataTable* InitializationData = nullptr;
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
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	//override IExAbilityProvider
	virtual void CollectAbilitCases(TArray<FExAbilityCase>& Abilities) const override;

	//��Ability Provider�Ĳ���
	virtual void RegisterAbilityProvider(IExAbilityProvider* ProviderObject);
	virtual void UnregisterAbilityProvider(IExAbilityProvider* ProviderObject);

	//ͨ��Case��Spec
	FGameplayAbilitySpec* FindAbilitySpecFromCase(const FExAbilityCase& AbilityCase);

public:

	UFUNCTION()
		void OnPawnControllerChanged(APawn* Pawn, AController* NewController);

	UFUNCTION(BlueprintCallable, Category = ExAbility)
		FGameplayAbilitySpecHandle GiveAbilityByCase(const FExAbilityCase& AbilityCase, UObject* AbilityProvider = nullptr);

	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation)
		virtual void TryActivateAbilityOnceWithEventData(const FExAbilityCase& AbilityCase, const FGameplayEventData& TriggerEventData, UObject* SourceObj);

	UFUNCTION(BlueprintCallable, Category = ExAbility)
		void TryActivateAbilityByCase(const FExAbilityCase& AbilityCase);

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
	FGameplayAbilitySpecHandle GiveAbilityByCaseInternal(const FExAbilityCase& AbilityCase, UObject* AbilityProvider);

};
