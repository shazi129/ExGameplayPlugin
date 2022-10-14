#pragma once

#include "UObject/UObjectGlobals.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameUIManager, Log, All);

class FGameUIManagerModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
};