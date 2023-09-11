#include "ExAttributeSet.h"
#include "GameplayEffectExtension.h"

FExOnAttributeChangeData::FExOnAttributeChangeData()
	: OldValue(0)
	, NewValue(0)
{
}

FExOnAttributeChangeData::FExOnAttributeChangeData(const FOnAttributeChangeData& ChangeData)
	: OldValue(ChangeData.OldValue)
	, NewValue(ChangeData.NewValue)
{
}

void FExOnAttributeChangeData::Set(float InOldValue, float InNewValue)
{
	OldValue = InOldValue;
	NewValue = InNewValue;
}


UExBaseAttributeSet::UExBaseAttributeSet()
{
}

void UExBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UExBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float CurrentMaxHealth = GetMaxHealth();
		if (!FMath::IsNearlyEqual(CurrentMaxHealth, 0.0f))
		{
			SetHealth(FMath::Clamp(GetHealth(), 0.0f, CurrentMaxHealth));
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		float CurrentMaxStamina = GetMaxStamina();
		if (!FMath::IsNearlyEqual(CurrentMaxStamina, 0.0f))
		{
			SetStamina(FMath::Clamp(GetStamina(), 0.0f, CurrentMaxStamina));
		}
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		float CurrentMaxMana = GetMaxMana();
		if (!FMath::IsNearlyEqual(CurrentMaxMana, 0.0f))
		{
			SetMana(FMath::Clamp(GetMana(), 0.0f, CurrentMaxMana));
		}
	}
}

void UExBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UExBaseAttributeSet::ORep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, Health, OldValue);
}

void UExBaseAttributeSet::ORep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, MaxHealth, OldValue);
}

void UExBaseAttributeSet::ORep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, Stamina, OldValue);
}

void UExBaseAttributeSet::ORep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, MaxStamina, OldValue);
}

void UExBaseAttributeSet::ORep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, Mana, OldValue);
}

void UExBaseAttributeSet::ORep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, MaxMana, OldValue);
}

