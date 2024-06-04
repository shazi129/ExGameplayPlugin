#pragma once

#include "Event/Handlers/GameplayEventHandler_ComponentBool.h"
#include "GameplayEventHandler_SetComponentVisibility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYUTILS_API UGameplayEventHandler_SetComponentVisibility : public UGameplayEventHandler_ComponentBool
{
	GENERATED_BODY()

public:
	virtual void HandleComponent_Implementation(UActorComponent* Component, bool Value) override;
};