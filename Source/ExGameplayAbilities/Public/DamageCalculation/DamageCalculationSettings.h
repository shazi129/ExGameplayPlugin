// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DamageCalculationSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Gameplay, defaultconfig, meta = (DisplayName = "DamageCalculation"))
class EXGAMEPLAYABILITIES_API UDamageCalculationSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override { return TEXT("Project"); }
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override { return TEXT("Game"); }
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override { return TEXT("DamageCalculation"); }

	UPROPERTY(config, EditAnywhere, Category=Configuration)
	float DamageBalanceCoefficient = 1;
};
