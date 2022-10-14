// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExGameplayLibrary, Log, All);

class FExGameplayLibraryModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define EXLIBRARY_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogExGameplayLibrary, Verbosity, Format, ##__VA_ARGS__); \
}