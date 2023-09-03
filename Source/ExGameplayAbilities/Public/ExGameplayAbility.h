#pragma once

#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "ExGameplayAbility.generated.h"


UCLASS(ClassGroup = (ExAbility), BlueprintType, Blueprintable, abstract, editinlinenew)
class EXGAMEPLAYABILITIES_API UExGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
		bool OnlyReplateWhenActivate = false;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAbilityCategory
{
	GENERATED_BODY()

	//技能所属的类别
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag CategoryTag;

	//技能在该类别中的优先级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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