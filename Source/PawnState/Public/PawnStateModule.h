// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPawnState, Log, All);

class FPawnStateModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define PAWNSTATE_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogPawnState, Verbosity, Format, ##__VA_ARGS__); \
}