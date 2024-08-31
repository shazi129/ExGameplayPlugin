#include "GameplayTypes.h"
#include "GameplayUtils.h"

bool FFilterActorCondition::FilterActor(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}
	if (!FilterIgnoreActors(Actor))
	{
		return false;
	}
	if (!FilterExcludeComponentClasses(Actor))
	{
		return false;
	}
	if (!FilterRequireComponentClasses(Actor))
	{
		return false;
	}
	if (!GameplayUtils::FilterActorClasses(Actor, ActorClasses))
	{
		return false;
	}

	return true;
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
