#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "ExGameplaySettings.generated.h"

UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "Gameplay"))
class EXGAMEPLAYPLUGIN_API UExGameplaySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UExGameplaySettings()
	{
		CategoryName = "Extension Gameplay";
	}

	UPROPERTY(config, EditAnywhere, Category = "Development", meta = (MetaClass = "CheatManagerExtension", DisplayName = "Cheat Manager Extension", ConfigRestartRequired = true))
		TArray<FSoftClassPath> CheatExtensionClasses;
};