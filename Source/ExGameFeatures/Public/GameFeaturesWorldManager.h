#pragma once

#include "CoreMinimal.h"
#include "GameFeaturesWorldManager.generated.h"


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

	bool IsBeginPlay();

public:
	static FWorldDelegates::FWorldEvent OnWorldBeginplayDelegate;

private:

	bool bIsInitialized = false;

	bool bIsWorldBeginplay = false;
};