#include "ExAttributeSet.h"
#include "GameplayEffectExtension.h"

UExBaseAttributeSet::UExBaseAttributeSet()
{
}

void UExBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UExBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UExBaseAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		float CurrentMaxHealth = GetMaxHealth();
		if (!FMath::IsNearlyEqual(CurrentMaxHealth, 0.0f))
		{
			NewValue = FMath::Clamp(NewValue, 0.0f, CurrentMaxHealth);
		}
	}
	else if (Attribute == GetStaminaAttribute())
	{
		float CurrentMaxStamina = GetMaxStamina();
		if (!FMath::IsNearlyEqual(CurrentMaxStamina, 0.0f))
		{
			NewValue = FMath::Clamp(NewValue, 0.0f, CurrentMaxStamina);
		}
	}
	else if (Attribute == GetManaAttribute())
	{
		float CurrentMaxMana = GetMaxMana();
		if (!FMath::IsNearlyEqual(CurrentMaxMana, 0.0f))
		{
			NewValue = FMath::Clamp(NewValue, 0.0f, CurrentMaxMana);
		}
	}
}

void UExBaseAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

bool UExBaseAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return true;
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
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExBaseAttributeSet, ManaRegenRate, COND_None, REPNOTIFY_Always);
}

void UExBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, Health, OldValue);
}

void UExBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, MaxHealth, OldValue);
}

void UExBaseAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, HealthRegenRate, OldValue);
}

void UExBaseAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, Stamina, OldValue);
}

void UExBaseAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, MaxStamina, OldValue);
}

void UExBaseAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, StaminaRegenRate, OldValue);
}

void UExBaseAttributeSet::OnRep_StaminaCostRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, StaminaCostRate, OldValue);
}

void UExBaseAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, Mana, OldValue);
}

void UExBaseAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, MaxMana, OldValue);
}

void UExBaseAttributeSet::OnRep_ManaRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExBaseAttributeSet, Mana, OldValue);
}

