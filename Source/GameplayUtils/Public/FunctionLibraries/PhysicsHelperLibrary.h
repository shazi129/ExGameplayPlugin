#pragma once

#include "CoreMinimal.h"
#include "PhysicsHelperLibrary.generated.h"

UCLASS()
class GAMEPLAYUTILS_API UPhysicsHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SetFixTick(bool Enable);

	UFUNCTION(BlueprintPure)
	static bool GetFixTick();
};