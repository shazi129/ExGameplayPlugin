#include "Attribute/ExAttributeTypes.h"
#include "ExGameplayLibrary.h"
#include "ExAbilitySystemComponent.h"

//void ExAttributeHelper::HandleAttributeChangeEvent(UAbilitySystemComponent* ASC, FAttributeChangeEventItem& EventItem, const FOnAttributeChangeData& Data)
//{
//	if (!UExGameplayLibrary::IsValidObject(ASC))
//	{
//		return;
//	}
//	if (EventItem.EffectClass == nullptr)
//	{
//		return;
//	}
//
//	bool NeedApply = false;
//	bool NeedRemove = false;
//
//	bool InServer = ASC->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer;
//	if (EventItem.EventMode == EEAttributeChangeEventMode::E_OnlyClient && InServer)
//	{
//		return;
//	}
//	else if (EventItem.EventMode == EEAttributeChangeEventMode::E_OnlyServer && !InServer)
//	{
//		return;
//	}
//
//	if (EventItem.EventType == EAttributeChangeEventType::E_Decrease)
//	{
//		NeedApply = Data.NewValue < Data.OldValue;
//	}
//	else if (EventItem.EventType == EAttributeChangeEventType::E_Increase)
//	{
//		NeedApply = Data.NewValue > Data.OldValue;
//	}
//	else if (EventItem.EventType == EAttributeChangeEventType::E_Threshold)
//	{
//		//向上还是向下的趋势
//		if (EventItem.ApplyThreshold > EventItem.RemoveThreshold) //向上
//		{
//			NeedApply = Data.NewValue >= EventItem.ApplyThreshold && Data.OldValue < EventItem.ApplyThreshold;
//			NeedRemove = Data.NewValue <= EventItem.RemoveThreshold && Data.OldValue > EventItem.RemoveThreshold;
//		}
//		else if (EventItem.ApplyThreshold < EventItem.RemoveThreshold) //向下
//		{
//			NeedApply = Data.NewValue <= EventItem.ApplyThreshold && Data.OldValue > EventItem.ApplyThreshold;
//			NeedRemove = Data.NewValue >= EventItem.RemoveThreshold && Data.OldValue < EventItem.RemoveThreshold;
//		}
//
//		//已经apply过了，没remove前不apply
//		if (NeedApply && EventItem.EffectHandle.WasSuccessfullyApplied())
//		{
//			NeedApply = false;
//		}
//	}
//
//	if (NeedApply)
//	{
//		int SameEffectCount = ASC->GetGameplayEffectCount(EventItem.EffectClass, ASC);
//		if (SameEffectCount > 0)
//		{
//			if (EventItem.MultiEffectPolicy == EMultiEffectPolicy::E_RemoveExist)
//			{
//				ASC->RemoveActiveGameplayEffectBySourceEffect(EventItem.EffectClass, ASC);
//			}
//			if (EventItem.MultiEffectPolicy != EMultiEffectPolicy::E_IgnoreNew)
//			{
//				EventItem.EffectHandle = GameplayEffectHelper::ApplyGameplayEffectClass(ASC, EventItem.EffectClass);
//			}
//		}
//		else
//		{
//			EventItem.EffectHandle = GameplayEffectHelper::ApplyGameplayEffectClass(ASC, EventItem.EffectClass);
//		}
//	}
//	else if (NeedRemove)
//	{
//		if (EventItem.EffectHandle.IsValid()) //instant effect不需要移除
//		{
//			ASC->RemoveActiveGameplayEffect(EventItem.EffectHandle);
//		}
//		EventItem.EffectHandle = FActiveGameplayEffectHandle();
//	}
//}

void FExAttributeConfig::Reset()
{
	DefaultAttributesClassList.Empty();
	InitAttributesEffectClass = nullptr;
	AttributeChangeEventMap.Empty();
}

FExOnAttributeChangeData::FExOnAttributeChangeData()
	: OldValue(0)
	, NewValue(0)
{
}

FExOnAttributeChangeData::FExOnAttributeChangeData(const FOnAttributeChangeData& ChangeData)
	: Attribute(ChangeData.Attribute)
	, OldValue(ChangeData.OldValue)
	, NewValue(ChangeData.NewValue)
{
}

void FExOnAttributeChangeData::Set(float InOldValue, float InNewValue)
{
	OldValue = InOldValue;
	NewValue = InNewValue;
}
