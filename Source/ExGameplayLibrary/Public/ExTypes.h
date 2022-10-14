#pragma once

#include "ExTypes.generated.h"

UENUM(Blueprintable)
enum class EBPNetMode : uint8
{
	Standalone = 0					UMETA(DisplayName = "Standalone"),
	DedicatedServer = 1			UMETA(DisplayName = "DedicatedServer"),
	ListenServer = 2				UMETA(DisplayName = "ListenServer"),
	Client = 3						UMETA(DisplayName = "Client"),

	MAX = 4							UMETA(DisplayName = "MAX"),
};

UENUM(Blueprintable)
enum class EBPLogLevel : uint8
{
	Log,
	Warning,
	Error
};