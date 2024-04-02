#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GameplayUtilSettings.generated.h"

UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "Gameplay Utility"))
class GAMEPLAYUTILS_API UGameplayUtilSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGameplayUtilSettings()
	{
		CategoryName = "Extension Gameplay";
	}

	UPROPERTY(config, EditAnywhere, Category = "Basic")
		bool EnableCheat = false;

	UPROPERTY(config, EditAnywhere, Category = "Basic", meta = (DisplayName = "Cheat Manager Extension", ConfigRestartRequired = true))
		TArray<TSoftClassPtr<UCheatManagerExtension>> CheatExtensionClasses;

	UPROPERTY(config, EditAnywhere, Category = "Global Variables")
		TMap<FName, bool> BoolVariables;

	UPROPERTY(config, EditAnywhere, Category = "Global Variables")
		TMap<FName, bool> IntVariables;

	UPROPERTY(config, EditAnywhere, Category = "Package")
		TArray<TSoftObjectPtr<UObject>> PermanentObjects;

	//������ҵ���Զ��建��ظ����߼�
	UPROPERTY(config, EditAnywhere, Category = "ObjectCache")
		TSoftClassPtr<class AObjectCacheReplicateActor> ObjectCacheReplicateActorClass;
};