#pragma once

#include "GameplayObject.h"
#include "Interact/InteractTypes.h"
#include "InteractItemHandler.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class GAMEPLAYUTILS_API UInteractItemHandler : public UGameplayObject
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInteractInstanceData& InstanceData)
	{
		Execute(InstanceData);
	}

	UFUNCTION(BlueprintImplementableEvent)
	void Execute(const FInteractInstanceData& InstanceData);
};