// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GeneralAsyncActionObject.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnLatentActionCompletedDelegate);

/**
 * 
 */
UCLASS()
class GAMEPLAYUTILS_API UGeneralAsyncActionObject : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FOnLatentActionCompletedDelegate OnLatentActionCompleted;

	UFUNCTION()
	void OnLatentActionCompletedFunc();
};
