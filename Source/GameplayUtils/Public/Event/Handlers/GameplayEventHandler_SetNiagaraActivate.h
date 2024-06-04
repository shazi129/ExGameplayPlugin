#pragma once

#pragma once

#include "Event/Handlers/GameplayEventHandler_ComponentBool.h"
#include "GameplayEventHandler_SetNiagaraActivate.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYUTILS_API UGameplayEventHandler_SetNiagaraActivate : public UGameplayEventHandler_ComponentBool
{
	GENERATED_BODY()

public:
	virtual void HandleComponent_Implementation(UActorComponent* Component, bool Value) override;
};