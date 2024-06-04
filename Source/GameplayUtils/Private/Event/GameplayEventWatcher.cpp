#include "Event/GameplayEventWatcher.h"

FGameplayEventContext::FGameplayEventContext(UObject* InInstigator)
{
	Instigator = InInstigator;
}

void UGameplayEventWatcher::Activate()
{
	ReceiveActivate();
}

void UGameplayEventWatcher::Deactivate()
{
	ReceiveDeactivate();
}


