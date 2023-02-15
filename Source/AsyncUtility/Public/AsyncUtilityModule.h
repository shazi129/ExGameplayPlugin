// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAsyncUtility, Log, All);

class FAsyncUtilityModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define ASYNCUTILITY_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogAsyncUtility, Verbosity, Format, ##__VA_ARGS__); \
}