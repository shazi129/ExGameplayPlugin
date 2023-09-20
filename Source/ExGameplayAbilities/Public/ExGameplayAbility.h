#pragma once

#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "ExGameplayAbility.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExAbilityDelegate, const UGameplayAbility*, Ability);


UCLASS(BlueprintType, Blueprintable)
class EXGAMEPLAYABILITIES_API UExGameplayAbilityChecker : public UObject
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintImplementableEvent)
	bool Check(const class UExGameplayAbility* Ability, const FGameplayAbilityActorInfo& ActorInfo) const;
};


UCLASS(ClassGroup = (ExAbility), BlueprintType, Blueprintable, abstract, editinlinenew)
class EXGAMEPLAYABILITIES_API UExGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	//激活失败通知
	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityFailed(const FGameplayTagContainer& FailTags) const;
	
	/** Checks cost. returns true if we can pay for the ability. False if not */
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/** Applies the ability's cost to the target */
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

public:
	//使用自定义的逻辑进行CheckCost
	UPROPERTY(EditAnywhere, Category = "Costs|Advanced", meta=(EditCondition="CostGameplayEffectClass != nullptr"))
		bool OverrideCostCheck = false;

	//CheckCost的自定义逻辑
	UPROPERTY(EditAnywhere, Category = "Costs|Advanced", meta = (EditCondition = "OverrideCostCheck && CostGameplayEffectClass != nullptr"))
		TArray<TSubclassOf<UExGameplayAbilityChecker>> OverrideCostCheckers;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAbilityCategory
{
	GENERATED_BODY()

	//技能所属的类别
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag CategoryTag;

	//技能在该类别中的优先级。 TODO: 目前优先级按注册时间来, 越晚越高
	UPROPERTY()
		int Priority;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FExAbilityCase
{
	GENERATED_BODY()

public:
#pragma region 配置数据
	//具体的技能
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		TSubclassOf<UGameplayAbility> AbilityClass;

	//该技能的归类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		TArray<FAbilityCategory> AbilityCategories;

	//技能的level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		int AbilityLevel = 1;

	//在give时，是否立即Activate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		bool ActivateWhenGiven = false;

#pragma endregion

	//当前是否可以激活
	UPROPERTY()
	bool CanActivate = true;

	//技能提供者
	UPROPERTY()
	TObjectPtr<UObject> SourceObject;

	FExAbilityCase(){}
	FExAbilityCase(TSubclassOf<UGameplayAbility> InAbilityClass);
	bool IsValid() const
	{
		return AbilityClass != nullptr;
	}
};