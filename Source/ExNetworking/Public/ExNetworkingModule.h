#pragma once

#include "UObject/UObjectGlobals.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExNetworking, Log, All);

class FExNetworkingModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
};

#define EXNETWORKING_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogExNetworking, Verbosity, Format, ##__VA_ARGS__); \
}