// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUIManager, Log, All);

class FUIManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define UIMANAGER_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogUIManager, Verbosity, Format, ##__VA_ARGS__); \
}