#pragma once 

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "PawnStateTriggerComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UPawnStateTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="PawnState")
	TArray<UPawnState*> PawnStates;
};
