// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExGameplayPluginModule.h"

DEFINE_LOG_CATEGORY(LogExGameplayPlugin);
#define LOCTEXT_NAMESPACE "FExGameplayPluginModule"

void FExGameplayPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FExGameplayPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExGameplayPluginModule, ExGameplayPlugin)