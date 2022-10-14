// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureBPLibrary.generated.h"


UENUM()
enum class EBPGameFeatureState : uint8
{
	Unloaded,
	Loaded,
	Deactivated,
	Activated,

	EGameFeatureState_Max UMETA(Hidden)
};

/**
 * 
 */
UCLASS()
class  UGameFeatureBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "GameFeatureBPLibrary")
	static UGameFeaturesSubsystem* GetGameFeatureSubsystem();

	UFUNCTION(BlueprintPure, Category = "GameFeatureBPLibrary")
	static bool AddToPluginsList( const FString& PluginFilename );
	
	UFUNCTION(BlueprintPure, Category = "GameFeatureBPLibrary")
	static FString GetPluginURLByName(const FString& PluginName);

	UFUNCTION(BlueprintCallable, Category = "GameFeatureBPLibrary")
	static void LoadGameFeature(const FString& PluginURL);

	UFUNCTION(BlueprintCallable, Category = "GameFeatureBPLibrary")
	static void ActiveGameFeature(const FString& PluginURL);
	
	UFUNCTION(BlueprintCallable, Category = "GameFeatureBPLibrary")
	static void LoadBuiltInGameFeaturePlugin(const FString& PluginName);

	UFUNCTION(BlueprintCallable, Category = "GameFeatureBPLibrary")
	static void LoadBuiltInGameFeaturePlugins();

	UFUNCTION(BlueprintCallable, Category = "GameFeatureBPLibrary")
	static void UnloadGameFeature(const FString& PluginURL, bool bKeepRegistered = false);

	UFUNCTION(BlueprintCallable, Category = "GameFeatureBPLibrary")
	static void DeactivateGameFeature(const FString& PluginURL);
	
	static void OnStatus(const UE::GameFeatures::FResult& InStatus);

	/* Enable or disable a game feature. */
	UFUNCTION(BlueprintCallable, Category = "GameFeatureBPLibrary")
	static void SetGameFeatureState(FString PluginURL, EBPGameFeatureState State);

};
