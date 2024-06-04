#include "Event/Handlers/GameplayEventHandler_SetNiagaraActivate.h"
#include "NiagaraComponent.h"

void UGameplayEventHandler_SetNiagaraActivate::HandleComponent_Implementation(UActorComponent* Component, bool Value)
{
	if (UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(Component))
	{
		if (Value)
		{
			NiagaraComponent->Activate();
		}
		else
		{
			NiagaraComponent->Deactivate();
		}
	}
}
