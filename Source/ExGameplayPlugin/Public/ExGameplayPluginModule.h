// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExGameplayPlugin, Log, All);

class FExGameplayPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define EXGAMEPLAY_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogExGameplayPlugin, Verbosity, Format, ##__VA_ARGS__); \
}