#pragma once

#include "CoreMinimal.h"
#include "GameFeaturesWorldManager.generated.h"


/** Base class for world system objects. */
UCLASS(Abstract, Blueprintable)
class EXGAMEFEATURES_API UGameFeatureWorldSystem : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void Initialize(const UObject* WorldContextObject);
};

/**
 * C++ WorldSubsystem to manage requested system instances
 * (ref counts requests to account for multiple feature requesting the same system).
 */
UCLASS()
class EXGAMEFEATURES_API UGameFeaturesWorldManager : public UWorldSubsystem
{
	GENERATED_BODY()

	//~ Begin UWorldSubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	//~ End UWorldSubsystem interface

public:
	UFUNCTION(BlueprintCallable, Category = "Game Features|World Systems", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "SystemType"))
		static UGameFeatureWorldSystem* FindGameFeatureWorldSystemOfType(const UObject* WorldContextObject, TSubclassOf<UGameFeatureWorldSystem> SystemType);

	UGameFeatureWorldSystem* RequestSystemOfType(TSubclassOf<UGameFeatureWorldSystem> SystemType);
	void ReleaseRequestForSystemOfType(TSubclassOf<UGameFeatureWorldSystem> SystemType);

	bool IsBeginPlay();

public:
	static FWorldDelegates::FWorldEvent OnWorldBeginplayDelegate;

private:
	UPROPERTY(transient)
		TMap<UGameFeatureWorldSystem*, int32> SystemInstances;

	bool bIsInitialized = false;

	bool bIsWorldBeginplay = false;
};