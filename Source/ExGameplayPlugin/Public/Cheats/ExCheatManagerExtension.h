#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "ExCheatManagerExtension.generated.h"

UCLASS()
class UExCheatManagerExtension : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void PrintStreamingLevelInfo();

	UFUNCTION(Exec)
	void ShowGlobals();
};