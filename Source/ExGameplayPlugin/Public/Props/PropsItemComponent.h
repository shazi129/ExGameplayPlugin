#pragma once


 #include "Components/ActorComponent.h"
 #include "ExAbilitySystemComponent.h"
 #include "PropsItemComponent.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FPropsItemInfo
{
	GENERATED_BODY()

	//道具类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PropsType;

	//道具ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PropsID;

	//道具数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int PropsCount = 1;
};

UCLASS(ClassGroup = (Props), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UPropsItemComponent : public UActorComponent, public IExAbilityProvider
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void CollectAbilitCases(TArray<FExAbilityCase>& Abilities) const override;

	UFUNCTION()
	virtual FAbilityProviderInfo GetProviderInfo() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
		FPropsItemInfo PropsInfo;

	//是否是实体道具，实体道具会一直挂在玩家身上
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
		bool InstancedProps = true;

	//用于加载本道具的技能，如果为空，使用默认的加载流程
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic")
		FExAbilityCase LoadPropsAbility;

	//用于卸载本道具的技能，如果为空，使用默认的卸载技能
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
		FExAbilityCase UnloadAbility;

	//拥有本道具后，玩家默认增加的技能
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		TArray<FExAbilityCase> PropsAbilities;
};