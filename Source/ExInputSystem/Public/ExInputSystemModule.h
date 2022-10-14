#pragma once

#include "UObject/UObjectGlobals.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(ExInputSystemLog, Log, All);

class FExInputSystemModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
};

#define EXINPUTSYSTEM_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(ExInputSystemLog, Verbosity, Format, ##__VA_ARGS__); \
}