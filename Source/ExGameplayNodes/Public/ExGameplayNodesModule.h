// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExGameplayNodes, Log, All);

class FExGameplayNodesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define EXGAMEPLAYNODES_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogExGameplayNodes, Verbosity, Format, ##__VA_ARGS__); \
}