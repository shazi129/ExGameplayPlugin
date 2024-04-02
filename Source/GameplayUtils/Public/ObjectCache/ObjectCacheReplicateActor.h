#pragma once

#include "CoreMinimal.h"
#include "ObjectCacheReplicateActor.generated.h"

UENUM(BlueprintType)
enum class EPoolObjectState : uint8
{
	E_None = 0,
	E_Create,
	E_Retain,
	E_Relase,
	E_Destroy,
};

UCLASS()
class GAMEPLAYUTILS_API AObjectCacheReplicateActor : public AActor
{
	GENERATED_BODY()

public:
	AObjectCacheReplicateActor();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastActorCreate(AActor* Actor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastActorRetain(AActor* Actor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastActorRelease(AActor* Actor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastActorDestroy(AActor* Actor);
};