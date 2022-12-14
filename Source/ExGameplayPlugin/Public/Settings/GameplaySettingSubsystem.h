#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplaySettingSubsystem.generated.h"

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UGameplaySettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UGameplaySettingSubsystem* GetGameplaySettingSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
		FString GetString(FString ConfigKey);

	UFUNCTION(BlueprintCallable)
		bool GetBool(FString ConfigKey);

	UFUNCTION(BlueprintCallable)
		int GetInt(FString ConfigKey);

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void HandleSettings();

	void HandleDefaultGameFeatures();

	void LoadAndAcitvateDefaultFeatures();

	void OnCheatManagerCreate(UCheatManager* CheatManager);

	void ParseCommandLine();

	void RegisterPostWorldInitHandler();
	void UnregisterPostWorldInitHandler();
	void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS);

private:

	TArray<FString> ActivatedGameFeaturesNames;

	FString ConfigFilePath;

	FDelegateHandle CheatManagerCreateHandle;

	FDelegateHandle PostWorldInitHandle;
};
