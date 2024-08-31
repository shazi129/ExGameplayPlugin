#pragma once

#include "GameFramework/OnlineSession.h"
#include "ExOnlineSession.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UExOnlineSession : public UOnlineSession
{
	GENERATED_BODY()

public:
	virtual void HandleDisconnect(UWorld* World, class UNetDriver* NetDriver);

	UFUNCTION(BlueprintNativeEvent)
	bool ReceiveDisconnect(UWorld* World, FName NetDriverName);
};