#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Engine/DeveloperSettings.h"
#include "ExUMGSettings.generated.h"


UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "UMG"))
class EXUMG_API UExUMGSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UExUMGSettings()
	{
		CategoryName = "Extension Gameplay";
	}

	UPROPERTY(Config, EditAnywhere)
	FString Desc;
};