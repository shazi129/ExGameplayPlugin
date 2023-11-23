#pragma once 

/**
 *  这个文件用于定义Attribute相关的事件
 * 
 *  ----------Attribute变化状态的定义------------
 *  基类： FAttributeEventCondition
 *  主要函数：
 *		bool CanApply(const FOnAttributeChangeData& Data, bool bDuringEvent) 当前是否能进入事件
 *		bool CanRemove(const FOnAttributeChangeData& Data, bool bDuringEvent) 当前是否需要退出事件
 * 
 *  派生出如下子类：
 *		FAttributeEventIncrease：  值增加的情况, 每增加一次触发一次，没有移除
 *		FAttributeEventDecrease：  值减少的情况，每减少一次触发一次，没有移除
 *		FAttributeEventIncreaseTo: 值增加到某个阈值
 *		FAttributeEventDecreaseTo: 值减少到某个阈值
 *		FAttributeEventThreshold： 值突破阈值的情况，其中ApplyThreshold表示Event需要触发的阈值，RemoveThreshold表示要退出的阈值
 * 
 * ---------Attribute事件逻辑的定义--------------
 * 基类： FAttributeChangeEvent
 * 主要函数：
 *		bool DuringEvent() 是否在事件运行中，如果一个事件可以多次运行，可以直接返回false
 *		void Apply(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data) 进入事件逻辑
 *		void Remove(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data) 退出事件逻辑
 *		void Handle(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data) 具体处理逻辑，一般不需要重载
 *
 * 派生出如下子类：
 *		FAttributeChangeEffectEvent： 进入时Apply一个GameplayEffect, 退出是把Effect移除。建议只有在FAttributeEventThreshold的情况才能使用Duration或Period的Effect
 *		FAttributeChangeStateEvent: 进入时Enter一个PawnState， 退出时Leave。建议只在FAttributeEventThreshold下使用
 * 
 * 
 * -------PawnState变化引起的Attribute事件---------------
 * 基类：FPawnStateAttributeEvent
 * 派生出如下子类：
 *		FAddAttributeEvent： 如果PawnState进入，对Attribute做增加
 * 
 */

#include "CoreMinimal.h"
#include "GameplayTypes.h"
#include "GameplayEffectTypes.h"
#include "AttributeEvent.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeEventCondition
{
	GENERATED_BODY()

	virtual ~FAttributeEventCondition(){};
	virtual bool CanApply(const FOnAttributeChangeData& Data, bool bDuringEvent) const
	{
		return false;
	}
	virtual bool CanRemove(const FOnAttributeChangeData& Data, bool bDuringEvent) const
	{
		return false;
	}
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeEventIncrease : public FAttributeEventCondition
{
	GENERATED_BODY()
	virtual ~FAttributeEventIncrease() {};
	virtual bool CanApply(const FOnAttributeChangeData& Data, bool bDuringEvent) const
	{
		return Data.NewValue > Data.OldValue;
	}
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeEventIncreaseTo : public FAttributeEventCondition
{
	GENERATED_BODY()

	//触发阈值
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Threshold;

	virtual ~FAttributeEventIncreaseTo() {};
	virtual bool CanApply(const FOnAttributeChangeData& Data, bool bDuringEvent) const
	{
		return Data.NewValue >= Threshold && Threshold >= Data.OldValue;
	}
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeEventDecreaseTo : public FAttributeEventCondition
{
	GENERATED_BODY()

	//触发阈值
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Threshold;

	virtual ~FAttributeEventDecreaseTo() {};
	virtual bool CanApply(const FOnAttributeChangeData& Data, bool bDuringEvent) const
	{
		return Data.NewValue <= Threshold && Threshold <= Data.OldValue;
	}
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeEventDecrease : public FAttributeEventCondition
{
	GENERATED_BODY()
	virtual ~FAttributeEventDecrease() {};
	virtual bool CanApply(const FOnAttributeChangeData& Data, bool bDuringEvent) const
	{
		return Data.NewValue < Data.OldValue;
	}
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeEventThreshold : public FAttributeEventCondition
{
	GENERATED_BODY()

	//触发阈值
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ApplyThreshold;

	//移除阈值
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RemoveThreshold;

	virtual ~FAttributeEventThreshold() {};
	virtual bool CanApply(const FOnAttributeChangeData& Data, bool bDuringEvent) const;
	virtual bool CanRemove(const FOnAttributeChangeData& Data, bool bDuringEvent) const;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeChangeEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EExecNetMode ExecNetMode = EExecNetMode::E_Server;

	//State相关事件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BaseStruct = "/Script/ExGameplayAbilities.AttributeEventCondition", ExcludeBaseStruct))
	FInstancedStruct Condition;
	
	virtual ~FAttributeChangeEvent(){};
	virtual bool DuringEvent(UAbilitySystemComponent* ASC) {return false;}
	virtual void Apply(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data){};
	virtual void Remove(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data){};
	virtual void Handle(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data);
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeChangeEventList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BaseStruct = "/Script/ExGameplayAbilities.AttributeChangeEvent", ExcludeBaseStruct))
	TArray<FInstancedStruct> EventList;
};

//应用多个Effect的策略
UENUM(BlueprintType)
enum class EMultiEffectPolicy : uint8
{
	//当相同的Effect来时，移除现有的Effect
	E_RemoveExist		UMETA(DisplayName = "Remove Exist"),

	//当相同的Effect来时，忽略新来的
	E_IgnoreNew		UMETA(DisplayName = "Ignore New"),
};

//用于处理当Attribute变化时，Apply或Remove一个GameplayEffect
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeChangeEffectEvent: public FAttributeChangeEvent
{
	GENERATED_BODY()

	//触发的GameplayEffect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Effect)
	TSubclassOf<UGameplayEffect> EffectClass;

	//触发Effect的策略
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	EMultiEffectPolicy MultiEffectPolicy = EMultiEffectPolicy::E_RemoveExist;

	virtual ~FAttributeChangeEffectEvent() {};
	virtual bool DuringEvent(UAbilitySystemComponent* ASC) override;
	virtual void Apply(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data) override;
	virtual void Remove(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data) override;

private:
	FActiveGameplayEffectHandle EffectHandle;
};


//用于处理当Attribute变化时，Enter或Leave一个PawnState
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeChangeStateEvent : public FAttributeChangeEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayThumbnail = "false"))
	TSoftObjectPtr<UPawnStateAsset> PawnStateAsset;

	virtual ~FAttributeChangeStateEvent() {};
	virtual bool DuringEvent(UAbilitySystemComponent* ASC) override;
	virtual void Apply(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data) override;
	virtual void Remove(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data) override;

private:
	int PawnStateID = 0;
};


//用于处理当PawnState变化时，设置Attribut的值
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FPawnStateAttributeEvent : public FPawnStateEventItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EExecNetMode EventMode = EExecNetMode::E_Server;

	UPROPERTY(EditAnywhere)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere)
	float Value;

	UPROPERTY(EditAnywhere)
	bool ResetWhenLeave;

	FPawnStateAttributeEvent()
	{
		ShowBasicConfig = false;
	}

	virtual ~FPawnStateAttributeEvent() {}

protected:
	bool Applied = false;
};

//用于处理当PawnState变化时，设置Attribut的值
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAddAttributeEvent : public FPawnStateAttributeEvent
{
	GENERATED_BODY()

	virtual ~FAddAttributeEvent() {};
	virtual void Execute(EPawnStateEventType InTriggerType, const FPawnStateInstance& PawnStateInstance, UPawnStateComponent* PawnStateComponnt);
};
