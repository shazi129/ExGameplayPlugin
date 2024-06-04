#include "Subsystems/GameplayInstanceSubsystem.h"
#include "Macros/SubsystemMacros.h"
#include "GameplayUtilsModule.h"

UGameplayInstanceSubsystem* UGameplayInstanceSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogGameplayUtils, UGameplayInstanceSubsystem, WorldContextObject);
}
