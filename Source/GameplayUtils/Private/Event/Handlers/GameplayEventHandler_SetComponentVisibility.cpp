#include "Event/Handlers/GameplayEventHandler_SetComponentVisibility.h"

void UGameplayEventHandler_SetComponentVisibility::HandleComponent_Implementation(UActorComponent* Component, bool Value)
{
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component))
	{
		PrimitiveComponent->SetVisibility(Value);
	}
}
