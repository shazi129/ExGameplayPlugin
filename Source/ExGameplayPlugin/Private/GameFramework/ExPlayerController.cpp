#include "GameFramework/ExPlayerController.h"
#include "Settings/ExGameplaySettings.h"

void AExPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if !UE_BUILD_SHIPPING
	AddCheats(true);
#endif
}
