// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExUMGModule.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ExUMGSettings.h"
#endif

DEFINE_LOG_CATEGORY(LogExUMG);
#define LOCTEXT_NAMESPACE "FExUMGModule"

void FExUMGModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	//RegisterSettings();
}

void FExUMGModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	//UnregisterSettings();
}

void FExUMGModule::RegisterSettings()
{
}

void FExUMGModule::UnregisterSettings()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExUMGModule, ExUMG)