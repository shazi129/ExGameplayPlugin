#include "Actions/GameFeatureAction_ActionBase.h"


FString UGameFeatureAction_ActionBase::ActionToString(UGameFeatureAction* Action)
{
	UGameFeatureAction_ActionBase* ExAction = Cast<UGameFeatureAction_ActionBase>(Action);
	if (ExAction)
	{
		return ExAction->ToString();
	}
	return GetNameSafe(Action);
}

FString UGameFeatureAction_ActionBase::ToString() const
{
	return GetNameSafe(this);
}