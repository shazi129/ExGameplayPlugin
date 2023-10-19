#pragma once

#include "GameplayTypes.h"
#include "RPCFunctionProvider.generated.h"

UINTERFACE(Blueprintable)
class GAMEPLAYUTILS_API URPCFunctionProvider : public UInterface
{
	GENERATED_BODY()
};

class GAMEPLAYUTILS_API IRPCFunctionProvider
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void SendMsgToServer(const FGameplayMessage& Message) = 0;

	UFUNCTION()
	virtual void SendMsgToClient(const FGameplayMessage& Message) = 0;
};
