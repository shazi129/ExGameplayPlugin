// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExUMG, Log, All);

class FExUMGModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define EXUMG_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogExUMG, Verbosity, Format, ##__VA_ARGS__); \
}