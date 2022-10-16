#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Engine/DeveloperSettings.h"
#include "ExUMGSettings.generated.h"


UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "Extension UMG"))
class EXUMG_API UExUMGSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UExUMGSettings()
	{
		CategoryName = "Game";
	}

	UPROPERTY(Config, EditAnywhere, Category = "Extension UMG")
	FString Desc;
};