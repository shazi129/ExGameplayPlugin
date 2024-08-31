#include "FunctionLibraries/PhysicsHelperLibrary.h"
#include "PhysicsEngine/PhysicsSettings.h"

void UPhysicsHelperLibrary::SetFixTick(bool Enable)
{
	UPhysicsSettings* PhysicsSettings = GetMutableDefault<UPhysicsSettings>();
	PhysicsSettings->bFixTick = Enable;
}

bool UPhysicsHelperLibrary::GetFixTick()
{
	return GetDefault<UPhysicsSettings>()->bFixTick;
}
