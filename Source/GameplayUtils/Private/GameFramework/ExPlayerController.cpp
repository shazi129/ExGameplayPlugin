#include "GameFramework/ExPlayerController.h"

void AExPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if !UE_BUILD_SHIPPING
	AddCheats(true);
#endif
}
