// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogZipUtility, Log, All);

class FZipUtilityModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define ZIPUTILITY_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogZipUtility, Verbosity, Format, ##__VA_ARGS__); \
}