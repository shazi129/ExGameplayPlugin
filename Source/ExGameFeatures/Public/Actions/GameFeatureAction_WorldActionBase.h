// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "Engine/World.h" // for FWorldDelegates::OnStartGameInstance
#include "GameFeatureAction_WorldActionBase.generated.h"

struct FWorldContext;

USTRUCT()
struct FGameFeatureActionWorldInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString WorldName;

	UPROPERTY();
	TObjectPtr<UWorld> WorldEntry;
};


/**
 * Base class for GameFeatureActions that wish to do something world specific.
 */
UCLASS(Abstract)
class UGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating() override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	void OnWorldBeginplay(UWorld* InWorld);
	void OnWorldTearDown(UWorld* InWorld);

private:
	virtual void AddToWorld(const FWorldContext& WorldContext) PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld,);

	bool IsWorldHandled(UWorld* InWorld);
	void HandleWorld(UWorld* InWorld);

	void AddHandledWorld(UWorld* InWorld);
	void RemoveHandledWord(UWorld* InWorld);

private:
	FDelegateHandle GameInstanceStartHandle;
	FDelegateHandle WorldCreateHandle;
	FDelegateHandle WorldBeginplayHandle;
	FDelegateHandle WorldTearDownHandle;

	TArray<FGameFeatureActionWorldInfo> HandledWorlds;
};
