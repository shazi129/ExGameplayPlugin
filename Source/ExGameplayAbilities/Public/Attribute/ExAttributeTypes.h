#pragma once

/**
 * 本文件定义了一些用于监听Attribute更改的数据结构
 */

#include "AttributeSet.h"
#include "PawnStateTypes.h"
#include "Attribute/AttributeEvent.h"
#include "ExAttributeTypes.generated.h"

////////////////////////////////////////////////////Attribut变化的一些事件//////////////////
USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FExAttributeConfig
{
	GENERATED_BODY()

	//默认的Attribute
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	TArray<TSubclassOf<UAttributeSet>> DefaultAttributesClassList;

	//初始化默认属性的Effect
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> InitAttributesEffectClass;

	//Attribute变化的事件
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes|ChangeEvent")
	TMap<FGameplayAttribute, FAttributeChangeEventList> AttributeChangeEventMap;

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
	FGameplayAttribute Attribute;

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


//namespace ExAttributeHelper
//{
//	void EXGAMEPLAYABILITIES_API HandleAttributeChangeEvent(UAbilitySystemComponent* ASC, FAttributeChangeEventItem& EventItem, const FOnAttributeChangeData& Data);
//}

