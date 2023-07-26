// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayUtils, Log, All);

class FGameplayUtilsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define GAMEPLAYUTILS_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogGameplayUtils, Verbosity, Format, ##__VA_ARGS__); \
}