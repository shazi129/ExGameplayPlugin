#include "GlobalEventSubsystem.h"
#include "GameplayUtilsModule.h"
#include "Macros/SubsystemMacros.h"

UGlobalEventSubsystem* UGlobalEventSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogGameplayUtils, UGlobalEventSubsystem, WorldContextObject);
}

void UGlobalEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ApplicationReactivatedHandle = FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &UGlobalEventSubsystem::OnApplicationReactivated);
}

void UGlobalEventSubsystem::Deinitialize()
{
	Super::Deinitialize();
	if (ApplicationReactivatedHandle.IsValid())
	{
		FCoreDelegates::ApplicationHasReactivatedDelegate.Remove(ApplicationReactivatedHandle);
	}
}

void UGlobalEventSubsystem::OnApplicationReactivated()
{
	GAMEPLAYUTILS_LOG(Log, TEXT("%s"), *FString(__FUNCTION__));
	if (ApplicationReactiveDelegate.IsBound())
	{
		ApplicationReactiveDelegate.Broadcast();
	}
}