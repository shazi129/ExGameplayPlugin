#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ExAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangeDelegate, const FExOnAttributeChangeData&,  ChangeData);

//
UCLASS(BlueprintType)
class EXGAMEPLAYABILITIES_API UOnAttributeValueChangeDelegateInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttributeValueChangeDelegate Delegate;

	//绑定到FOnGameplayAttributeValueChange的回调，用于析构时解绑
	FDelegateHandle BindHandle;
};

UCLASS()
class UExAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
};

/**
 * 一个玩家基础的Attribute, 包括血量，体力值和魔法值
 */
UCLASS()
class UExBaseAttributeSet : public UExAttributeSet
{
	GENERATED_BODY()

public:
	UExBaseAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	virtual void ORep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void ORep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void ORep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void ORep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void ORep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void ORep_MaxMana(const FGameplayAttributeData& OldValue);

public:
	//生命值
	UPROPERTY(BlueprintReadOnly, Category="Health", ReplicatedUsing=ORep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UExBaseAttributeSet, Health)

	//最大生命值
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = ORep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UExBaseAttributeSet, MaxHealth)

	//体力值
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = ORep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UExBaseAttributeSet, Stamina)

	//最大体力值
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = ORep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UExBaseAttributeSet, MaxStamina)

	//魔法值
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = ORep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UExBaseAttributeSet, Mana)

	//最大魔法值
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = ORep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UExBaseAttributeSet, MaxMana)
};