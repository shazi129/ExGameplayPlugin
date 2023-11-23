#include "Attribute/AttributeEvent.h"
#include "ExAbilitySystemComponent.h"
#include "ExGameplayAbilityLibrary.h"
#include "PawnStateComponent.h"
#include "PawnStateLibrary.h"
#include "PawnStateSubsystem.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"


bool FAttributeEventThreshold::CanApply(const FOnAttributeChangeData& Data, bool bDuringEvent) const
{
	if (bDuringEvent)
	{
		return false;
	}

	bool bCanApply = false;
	if (ApplyThreshold > RemoveThreshold) //向上
	{
		bCanApply = Data.NewValue >= ApplyThreshold && Data.OldValue < ApplyThreshold;
	}
	else if (ApplyThreshold < RemoveThreshold) //向下
	{
		bCanApply = Data.NewValue <= ApplyThreshold && Data.OldValue > ApplyThreshold;
	}

	return bCanApply;
}

bool FAttributeEventThreshold::CanRemove(const FOnAttributeChangeData& Data, bool bDuringEvent) const
{
	if (!bDuringEvent)
	{
		return false;
	}

	if (ApplyThreshold > RemoveThreshold) //向上
	{
		return Data.NewValue <= RemoveThreshold && Data.OldValue > RemoveThreshold;
	}
	else if (ApplyThreshold < RemoveThreshold) //向下
	{
		return Data.NewValue >= RemoveThreshold && Data.OldValue < RemoveThreshold;
	}
	return false;
}

void FAttributeChangeEvent::Handle(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data)
{
	if (!ASC || !Data.Attribute.IsValid() || !Condition.IsValid())
	{
		return;
	}

	if (!UGameplayUtilsLibrary::CheckExecNetMode(ASC, ExecNetMode))
	{
		return;
	}

	const UScriptStruct* ConditionType = Condition.GetScriptStruct();
	if (!ConditionType->IsChildOf(FAttributeEventCondition::StaticStruct()))
	{
		return;
	}

	FAttributeEventCondition& EventCondition = Condition.GetMutable<FAttributeEventCondition>();
	if (EventCondition.CanApply(Data, DuringEvent(ASC)))
	{
		Apply(ASC, Data);
	}
	else if (EventCondition.CanRemove(Data, DuringEvent(ASC)))
	{
		Remove(ASC, Data);
	}
}

bool FAttributeChangeEffectEvent::DuringEvent(UAbilitySystemComponent* ASC)
{
	int SameEffectCount = ASC->GetGameplayEffectCount(EffectClass, ASC);
	return SameEffectCount > 0;
}

void FAttributeChangeEffectEvent::Apply(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data)
{
	if (!EffectClass)
	{
		return;
	}

	int SameEffectCount = ASC->GetGameplayEffectCount(EffectClass, ASC);
	if (SameEffectCount > 0)
	{
		if (MultiEffectPolicy == EMultiEffectPolicy::E_RemoveExist)
		{
			ASC->RemoveActiveGameplayEffectBySourceEffect(EffectClass, ASC);
		}
		if (MultiEffectPolicy != EMultiEffectPolicy::E_IgnoreNew)
		{
			EffectHandle = UExGameplayAbilityLibrary::ApplyGameplayEffectClass(ASC, EffectClass);
		}
	}
	else
	{
		EffectHandle = UExGameplayAbilityLibrary::ApplyGameplayEffectClass(ASC, EffectClass);
	}
}

void FAttributeChangeEffectEvent::Remove(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data)
{
	ASC->RemoveActiveGameplayEffect(EffectHandle);
	EffectHandle = FActiveGameplayEffectHandle();
}

void FAddAttributeEvent::Execute(EPawnStateEventType InTriggerType, const FPawnStateInstance& PawnStateInstance, UPawnStateComponent* PawnStateCompnent)
{
	if (!PawnStateCompnent && Value != 0)
	{
		return;
	}

	//不用Reset就不需要在Leve的时候做处理
	if (InTriggerType == EPawnStateEventType::E_Leave && !ResetWhenLeave)
	{
		return;
	}

	//客户端还是服务侧
	if (!UGameplayUtilsLibrary::CheckExecNetMode(PawnStateCompnent, EventMode))
	{
		return;
	}

	AActor* Owner = PawnStateCompnent->GetOwner();
	if (!Owner || Owner->GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	{
		return;
	}
	UExAbilitySystemComponent* ASC = Cast<UExAbilitySystemComponent>(Owner->GetComponentByClass(UAbilitySystemComponent::StaticClass()));
	if (!ASC)
	{
		return;
	}

	//进入事件
	if (InTriggerType == EPawnStateEventType::E_Enter)
	{
		if (!Applied || !ResetWhenLeave)
		{
			ASC->ApplyModifyToAttribute(Attribute, EGameplayModOp::Type::Additive, Value);
			Applied = true;
		}
	}
	else if (InTriggerType == EPawnStateEventType::E_Leave)
	{
		if (Applied && ResetWhenLeave)
		{
			ASC->ApplyModifyToAttribute(Attribute, EGameplayModOp::Type::Additive, -Value);
			Applied = false;
		}
	}
}

bool FAttributeChangeStateEvent::DuringEvent(UAbilitySystemComponent* ASC)
{
	return PawnStateID > 0;
}

void FAttributeChangeStateEvent::Apply(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data)
{
	UPawnStateAsset* Asset = nullptr;
	if (!PawnStateAsset.IsNull())
	{
		Asset = PawnStateAsset.LoadSynchronous();
	}
	if (!UPawnStateSubsystem::GetSubsystem(ASC)->LoadPawnStateAsset(Asset))
	{
		return;
	}

	UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(ASC->GetOwner());
	if (PawnStateComponent )
	{
		PawnStateID = PawnStateComponent->TryEnterPawnState(Asset->StateTag, ASC);
	}
}

void FAttributeChangeStateEvent::Remove(UAbilitySystemComponent* ASC, const FOnAttributeChangeData& Data)
{
	UPawnStateAsset* Asset = nullptr;
	if (!PawnStateAsset.IsNull())
	{
		Asset = PawnStateAsset.LoadSynchronous();
	}
	if (!UPawnStateSubsystem::GetSubsystem(ASC)->LoadPawnStateAsset(Asset))
	{
		return;
	}

	UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(ASC->GetOwner());
	if (PawnStateComponent)
	{
		PawnStateComponent->LeavePawnState(PawnStateID, ASC);
		PawnStateID = 0;
	}
}

