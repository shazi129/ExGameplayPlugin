// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExGameFeatures, Log, All);

class FExGameFeaturesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define EXIGAMEFEATURE_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogExGameFeatures, Verbosity, Format, ##__VA_ARGS__); \
}