#include "Components/ExChildActorComponent.h"

void UExChildActorComponent::CreateChildActor(TFunction<void(AActor*)> CustomizerFunc)
{
	if (UWorld* World = this->GetWorld())
	{
		if (DoNotSpawnInDedicatedServer && World->GetNetMode() == ENetMode::NM_DedicatedServer)
		{
			return;
		}
	}

	Super::CreateChildActor(CustomizerFunc);
}