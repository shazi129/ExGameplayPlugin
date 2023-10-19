#include "GameplayTypes.h"

bool FFilterActorCondition::FilterActorClasses(AActor* Actor) const
{
	if (!Actor) return false;
	if (ActorClasses.IsEmpty())
	{
		return true;
	}
	for (auto& ActorClass : ActorClasses)
	{
		if (Actor->IsA(ActorClass))
		{
			return true;
		}
	}
	return false;
}

bool FFilterActorCondition::FilterExcludeComponentClasses(AActor* Actor) const
{
	if (!Actor) return false;
	for (auto& ExcludeComponentClass : ExcludeComponentClasses)
	{
		if (Actor->GetComponentByClass(ExcludeComponentClass))
		{
			return false;
		}
	}
	return true;
}

bool FFilterActorCondition::FilterRequireComponentClasses(AActor* Actor) const
{
	if (!Actor) return false;
	if (RequireComponentClasses.IsEmpty())
	{
		return true;
	}

	bool HasFound = false;
	for (auto& RequireComponentClass : RequireComponentClasses)
	{
		if (Actor->GetComponentByClass(RequireComponentClass))
		{
			HasFound = true;
			break;
		}
	}
	return HasFound;
}

bool FFilterActorCondition::FilterIgnoreActors(AActor* Actor) const
{
	if (!Actor) return false;
	if (IgnoreActors.IsEmpty() || !IgnoreActors.Contains(Actor))
	{
		return true;
	}
	return false;
}

FString FGameplayMessage::ToString() const
{
	return FString::Printf(TEXT("Tag:%s, BodyType:%s"), *MsgTag.ToString(), *GetNameSafe(MsgBody.GetScriptStruct()));
}
