#pragma once
#include "GameFramework/PlayerController.h"
#include "ExPlayerController.generated.h"

UCLASS()
class GAMEPLAYUTILS_API AExPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;
};
