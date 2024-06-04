// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncActions/GeneralAsyncActionObject.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncActionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYUTILS_API UAsyncActionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UAsyncActionSubsystem* GetSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	UGeneralAsyncActionObject* CreateVoidActionObject(UObject* WorldContextObject);

	void RemoveActionObject(UObject* Object);
	
private:
	UPROPERTY()
	TSet<UObject*> AsyncObjectSet;
};
