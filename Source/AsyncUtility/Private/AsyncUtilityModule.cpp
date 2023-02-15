// Copyright Epic Games, Inc. All Rights Reserved.

#include "AsyncUtilityModule.h"

#define LOCTEXT_NAMESPACE "AsyncUtilityModule"

DEFINE_LOG_CATEGORY(LogAsyncUtility)

void FAsyncUtilityModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

}

void FAsyncUtilityModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAsyncUtilityModule, AsyncUtility)