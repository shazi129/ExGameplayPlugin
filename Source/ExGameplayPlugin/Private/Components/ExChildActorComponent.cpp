#include "Components/ExChildActorComponent.h"

void UExChildActorComponent::CreateChildActor()
{
	if (UWorld* World = this->GetWorld())
	{
		if (DoNotSpawnInDedicatedServer && World->GetNetMode() == ENetMode::NM_DedicatedServer)
		{
			return;
		}
	}

	Super::CreateChildActor();
}