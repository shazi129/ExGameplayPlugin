#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GameplaySettings.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FDevelopModule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		FString ModelName;

	UPROPERTY(EditAnywhere)
		TArray<FString> SubModuleNames;
};

UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "Gameplay"))
class EXGAMEPLAYPLUGIN_API UGameplaySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGameplaySettings()
	{
		CategoryName = "Extension Gameplay";
	}

	UPROPERTY(Config, EditAnywhere, Category = "GameFeatures")
		TArray<FString> DefaultGameFeatures;

	UPROPERTY(Config, EditAnywhere, Category = "Development")
		TArray<FDevelopModule> DevelopModules;

	UPROPERTY(config, EditAnywhere, Category = "Development", meta = (MetaClass = "CheatManagerExtension", DisplayName = "Cheat Manager Extension", ConfigRestartRequired = true))
		TArray<FSoftClassPath> CheatExtensionClasses;
};