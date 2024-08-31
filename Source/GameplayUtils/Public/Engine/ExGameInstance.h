#pragma once

#include "CoreMinimal.h"
#include "ExGameInstance.generated.h"

/**
 * 对引擎GameInstance的扩展
 */

UCLASS()
class GAMEPLAYUTILS_API UExGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//
	virtual TSubclassOf<UOnlineSession> GetOnlineSessionClass();
};