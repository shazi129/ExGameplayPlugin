#include "GameFramework/ExOnlineSession.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"

void UExOnlineSession::HandleDisconnect(UWorld* World, UNetDriver* NetDriver)
{
	if (!ReceiveDisconnect(World, NetDriver ? NetDriver->NetDriverName : NAME_None))
	{
		Super::HandleDisconnect(World, NetDriver);
	}
}

bool UExOnlineSession::ReceiveDisconnect_Implementation(UWorld* Worldr, FName NetDriverName)
{
	return false;
}
