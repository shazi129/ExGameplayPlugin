#pragma once

/**
 * 本文件定义了一些用于监听Attribute更改的数据结构
 */

#include "AttributeSet.h"
#include "PawnStateTypes.h"
#include "AbilitySystemComponent.h"
#include "ActiveGameplayEffectHandle.h"
#include "ExAttributeTypes.generated.h"

 //Attribute变化类型
UENUM(BlueprintType)
enum class EAttributeChangeEventType : uint8
{
	//阈值类型，到一个阈值触发，到另一个阈值结束
	E_Threshold		UMETA(DisplayName = "Threshold"),

	//增加类型，Attribute增加就触发一次
	E_Increase			UMETA(DisplayName = "Increase"),

	//减少类型，Attribute减少就触发一次
	E_Decrease			UMETA(DisplayName = "Decrease"),
};

//Attribute变化事件触发模式
UENUM(BlueprintType)
enum class EEAttributeChangeEventMode : uint8
{
	//只在客户端触发
	E_OnlyClient		UMETA(DisplayName = "OnlyClient"),

	//只在服务端触发
	E_OnlyServer			UMETA(DisplayName = "OnlyServer"),

	//两端都触发
	E_Both			UMETA(DisplayName = "Both"),
};

//应用多个Effect的策略
UENUM(BlueprintType)
enum class EMultiEffectPolicy : uint8
{
	//当相同的Effect来时，移除现有的Effect
	E_RemoveExist		UMETA(DisplayName = "Remove Exist"),

	//当相同的Effect来时，忽略新来的
	E_IgnoreNew			UMETA(DisplayName = "Ignore New"),
};

//Attribut变化的一些事件
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeChangeEventItem
{
	GENERATED_BODY()

	//触发的GameplayEffect
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEAttributeChangeEventMode EventMode = EEAttributeChangeEventMode::E_OnlyServer;

	//触发Effect的策略
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMultiEffectPolicy MultiEffectPolicy = EMultiEffectPolicy::E_RemoveExist;

	//监听的事件类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttributeChangeEventType EventType = EAttributeChangeEventType::E_Threshold;

	//触发阈值
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EventType == EAttributeChangeEventType::E_Threshold", EditConditionHides))
	float ApplyThreshold;

	//移除阈值
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EventType == EAttributeChangeEventType::E_Threshold", EditConditionHides))
	float RemoveThreshold;

#pragma region /////////////////////内部逻辑///////////////////
	//对应Effect的Handle
	FActiveGameplayEffectHandle EffectHandle;
#pragma endregion
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAttributeChangeEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAttributeChangeEventItem> EventItemList;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FExAttributeConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	TArray<TSubclassOf<UAttributeSet>> DefaultAttributesClassList;

	//初始化默认属性的Effect
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> InitAttributesEffectClass;

	//Attribute变化的事件
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes|ChangeEvent")
	TMap<FGameplayAttribute, FAttributeChangeEvent> AttributeChangeEventMap;

	void Reset();
};


//Attribut配置
UCLASS(BlueprintType)
class EXGAMEPLAYABILITIES_API UExAttributeConfigAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FExAttributeConfig AttributeConfig;
};

/**
 * 对FOnAttributeChangeData的扩展，使其可以在蓝图使用
 */
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FExOnAttributeChangeData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float OldValue;

	UPROPERTY(BlueprintReadOnly)
	float NewValue;

	FExOnAttributeChangeData();
	FExOnAttributeChangeData(const FOnAttributeChangeData& ChangeData);
	void Set(float InOldValue, float InNewValue);
};

//定义一个可以通知到蓝图的Attribute更改回调
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangeDelegate, const FExOnAttributeChangeData&, ChangeData);

//
UCLASS(BlueprintType)
class EXGAMEPLAYABILITIES_API UOnAttributeValueChangeDelegateInfo : public UObject
{
	GENERATED_BODY()

public:
	//用于给外部注册的回调
	UPROPERTY(BlueprintAssignable)
	FOnAttributeValueChangeDelegate Delegate;

	//上次变更的数值
	float OldValue = -1;

	//绑定到FOnGameplayAttributeValueChange的回调，用于析构时解绑
	FDelegateHandle BindHandle;
};


namespace ExAttributeHelper
{
	void EXGAMEPLAYABILITIES_API HandleAttributeChangeEvent(UAbilitySystemComponent* ASC, FAttributeChangeEventItem& EventItem, const FOnAttributeChangeData& Data);
}

namespace GameplayEffectHelper
{
	FActiveGameplayEffectHandle EXGAMEPLAYABILITIES_API ApplyGameplayEffectClass(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> EffectClass);
}

//用于处理当PawnState变化时，设置Attribut的值
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FPawnStateAttributeEvent : public FPawnStateEventItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEAttributeChangeEventMode EventMode = EEAttributeChangeEventMode::E_OnlyServer;

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

	virtual ~FPawnStateAttributeEvent(){}

protected:
	bool Applied = false;
};

//用于处理当PawnState变化时，设置Attribut的值
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FAddAttributeEvent : public FPawnStateAttributeEvent
{
	GENERATED_BODY()

	virtual ~FAddAttributeEvent(){};
	virtual void Execute(EPawnStateEventTriggerType InTriggerType, const FPawnStateInstance& PawnStateInstance, UPawnStateComponent* PawnStateComponnt);
};
