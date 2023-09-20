// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageCalculation/DamageCalculationExecution.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExAttributeSet.h"
#include "DamageCalculation/DamageCalculationSettings.h"


struct DamageCalculationStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Stamina);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	DamageCalculationStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UExBaseAttributeSet, Stamina, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UExBaseAttributeSet, Health, Target, false);
	}
};

static const DamageCalculationStatics& DamageStatics()
{
	static DamageCalculationStatics DmgStatics;
	return DmgStatics;
}

UDamageCalculationExecution::UDamageCalculationExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().StaminaDef);
}

void UDamageCalculationExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const float AttackCoefficient = Spec.GetSetByCallerMagnitude(FName(TEXT("AttackCoefficient")));
	const float HitCoefficient = Spec.GetSetByCallerMagnitude(FName(TEXT("HitCoefficient")));
	const float RelativeVelocity = Spec.GetSetByCallerMagnitude(FName(TEXT("RelativeVelocity")));
	const float Weight = Spec.GetSetByCallerMagnitude(FName(TEXT("Weight")));

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float AttackerStamina = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StaminaDef, EvaluationParameters, AttackerStamina);
	if (AttackerStamina == 0.0f)
	{
		AttackerStamina = 1.0f;
	}

	const UDamageCalculationSettings* Settings = GetDefault<UDamageCalculationSettings>();
	const float DamageBalanceCoefficient = Settings->DamageBalanceCoefficient;

	float DamageDone = AttackCoefficient * HitCoefficient * RelativeVelocity * AttackerStamina * DamageBalanceCoefficient;
	UE_LOG(LogTemp, Log, TEXT("DamageCalculationExecution, AttackCoefficient: %f,HitCoefficient: %f,RelativeVelocity: %f,Weight: %f,AttackerStamina: %f,DamageBalanceCoefficient: %f,DamageDone: %f"),
	       AttackCoefficient, HitCoefficient, RelativeVelocity, Weight, AttackerStamina, DamageBalanceCoefficient, DamageDone)
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -DamageDone));
}
