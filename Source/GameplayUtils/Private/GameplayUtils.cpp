#include "GameplayUtils.h"

bool GameplayUtils::FilterActorClasses(AActor* Actor, const TArray<TSubclassOf<AActor>>& ActorClasses)
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
