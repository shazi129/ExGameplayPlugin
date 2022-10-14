// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExGameplayAbilities, Log, All);

class FExGameplayAbilitiesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};


#define EXABILITY_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogExGameplayAbilities, Verbosity, Format, ##__VA_ARGS__); \
}
