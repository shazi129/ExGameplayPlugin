// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCacheSystem, Log, All);

class FCacheSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define CACHESYSTEM_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogCacheSystem, Verbosity, Format, ##__VA_ARGS__); \
}