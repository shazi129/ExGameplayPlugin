// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHttpUtility, Log, All);

class FHttpUtilityModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define HTTPUTILITY_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogHttpUtility, Verbosity, Format, ##__VA_ARGS__); \
}