#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PawnStateCheatExtension.generated.h"

UCLASS()
class PAWNSTATE_API UPawnStateCheatExtension : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
		void DebugPawnState();
};