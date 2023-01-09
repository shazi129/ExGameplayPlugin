#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PawnStateCheatExtension.generated.h"

UCLASS()
class UPawnStateCheatExtension : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
		void DebugPawnState();
};