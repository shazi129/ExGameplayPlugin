#pragma once

#include "CoreMinimal.h"
#include "PawnState/PawnStateTypes.h"
#include "ExGameplayPluginSettings.generated.h"

UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "External Gameplay Setting"))
class EXGAMEPLAYPLUGIN_API UExGameplayPluginSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UExGameplayPluginSettings() 
	{
		CategoryName = "Extension Gameplay";
	}

	UPROPERTY(Config, EditAnywhere, Category = "Development")
		TArray<FPawnStateConfigItem> PawnStateConfig;
};