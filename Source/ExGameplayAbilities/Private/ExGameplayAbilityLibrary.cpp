#include "ExGameplayAbilityLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "ExGameplayAbilityTargetTypes.h"

TArray<int64> UExGameplayAbilityLibrary::GetInt64ArrayFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index)
{
	TArray<int64> Result;

	if (Index >= 0 && TargetData.Num() > Index)
	{
		const FGameplayAbilityTargetData_Int64Array* Int64ArrayData = static_cast<const FGameplayAbilityTargetData_Int64Array*>(TargetData.Get(Index));
		if (Int64ArrayData != nullptr)
		{
			Result = Int64ArrayData->Int64Array;
		}
	}

	return Result;
}

FInstancedStruct UExGameplayAbilityLibrary::GetInstancedStructFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index)
{
	if (Index >= 0 && TargetData.Num() > Index)
	{
		const FGameplayAbilityTargetData_FInstancedStruct* InstancedStruct = static_cast<const FGameplayAbilityTargetData_FInstancedStruct*>(TargetData.Get(Index));
		if (InstancedStruct != nullptr)
		{
			return InstancedStruct->InstancedStruct;
		}
	}
	return FInstancedStruct();
}

bool UExGameplayAbilityLibrary::IsAbilityCaseValid(const FExAbilityCase& AbilityCase)
{
	return AbilityCase.IsValid();
}

UAbilitySystemComponent* UExGameplayAbilityLibrary::GetInstigatorASCWithGESpec(const FGameplayEffectSpec& Spec)
{
	const FGameplayEffectContextHandle& Context = Spec.GetContext();
	return Context.GetInstigatorAbilitySystemComponent();
}

FActiveGameplayEffectHandle UExGameplayAbilityLibrary::ApplyGameplayEffectToSelfAndSetSourceObject(UAbilitySystemComponent* Target, TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle EffectContext,
                                                                                                   AActor* Attacker, float InAttackCoefficient, float InHitCoefficient, float InRelativeVelocity, float InWeight)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("ApplyGameplayEffect Target is Null"));
		return FActiveGameplayEffectHandle();
	}
	if (!Attacker)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyGameplayEffect Attacker is Null"));
	}
	if (!EffectContext.IsValid())
	{
		if (Attacker)
		{
			UAbilitySystemComponent* SourceASC = static_cast<UAbilitySystemComponent*>(Attacker->GetComponentByClass(UAbilitySystemComponent::StaticClass()));
			if (SourceASC)
			{
				EffectContext = SourceASC->MakeEffectContext();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ApplyGameplayEffect AttackerASC is Null"));
			}
		}
	}
	if (!EffectContext.IsValid())
	{
		EffectContext = Target->MakeEffectContext();
	}
	FGameplayEffectSpecHandle Spec = Target->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContext);
	UAbilitySystemBlueprintLibrary::AssignSetByCallerMagnitude(Spec, FName(TEXT("AttackCoefficient")), InAttackCoefficient);
	UAbilitySystemBlueprintLibrary::AssignSetByCallerMagnitude(Spec, FName(TEXT("HitCoefficient")), InHitCoefficient);
	UAbilitySystemBlueprintLibrary::AssignSetByCallerMagnitude(Spec, FName(TEXT("RelativeVelocity")), InRelativeVelocity);
	UAbilitySystemBlueprintLibrary::AssignSetByCallerMagnitude(Spec, FName(TEXT("Weight")), InWeight);
	return Target->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}
