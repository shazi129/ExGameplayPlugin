// Copyright Epic Games, Inc. All Rights Reserved.

#include "UIManagerModule.h"

#define LOCTEXT_NAMESPACE "UIManagerModule"

DEFINE_LOG_CATEGORY(LogUIManager)

void FUIManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

}

void FUIManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUIManagerModule, UIManager)