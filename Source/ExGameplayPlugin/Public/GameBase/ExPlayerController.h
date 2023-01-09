#pragma once
#include "GameFramework/PlayerController.h"
#include "ExPlayerController.generated.h"

UCLASS()
class AExPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void AddCheats(bool bForce = false);
};
