#include "ExGameFeaturesCheat.h"
#include "ExGameFeaturesModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ExGameFeaturesSubsystem.h"

FGameplayTag UExGameFeaturesCheat::ParseParamTagValue(const FString& Param, const FString& Name)
{
	FString ValueString;
	bool Result = UKismetSystemLibrary::ParseParamValue(Param, *Name, ValueString);
	if (Result)
	{
		return UExGameplayLibrary::RequestGameplayTag(FName(ValueString), false);
	}
	else
	{
		return FGameplayTag();
	}
}


void UExGameFeaturesCheat::ExGF(const FString& Param)
{
	FString ValueString;
	if (UKismetSystemLibrary::ParseParam(Param, "ShowModularActions"))
	{
		ShowModularActions();
	}
	else if (UKismetSystemLibrary::ParseParamValue(Param, "-Activate=", ValueString))
	{
		ActivateActions(ValueString);
	}
	else if (UKismetSystemLibrary::ParseParamValue(Param, "-Deactivate=", ValueString))
	{
		DeactivateActions(ValueString);
	}
	else
	{
		ShowUsage();
	}
}


void UExGameFeaturesCheat::ShowModularActions()
{
	const auto& ModularActionsMap = UExGameFeaturesSubsystem::GetSubsystem(this)->GetModularActions();

	FString Content = FString::Printf(TEXT("Total %d ModularActions:\n"), ModularActionsMap.Num());
	for (const auto& ModularActionsItem : ModularActionsMap)
	{
		Content.Append(FString::Printf(TEXT("\t%s --> %s\n"), *ModularActionsItem.Key.ToString(), *ModularActionsItem.Value.ToString()));
	}
	EXIGAMEFEATURE_LOG(Error, TEXT("%s: %s"), *FString(__FUNCTION__), *Content);
}

void UExGameFeaturesCheat::ShowUsage()
{
	FString Usage;
	Usage.Append(FString::Printf(TEXT("\nExGF -ShowModularActions\n")));
	Usage.Append(FString::Printf(TEXT("\t-Activate=A.B.C  -Deactivate=A.B.C\n")));
	EXIGAMEFEATURE_LOG(Error, TEXT("%s: %s"), *FString(__FUNCTION__), *Usage);
}

void UExGameFeaturesCheat::ActivateActions(const FString& Tag)
{
	FGameplayTag ModuleTag = UExGameplayLibrary::RequestGameplayTag(FName(Tag), false);
	if (ModuleTag.IsValid())
	{
		UExGameFeaturesSubsystem::GetSubsystem(this)->ActivateModule(ModuleTag);
	}
	else
	{
		EXIGAMEFEATURE_LOG(Error, TEXT("%s error, Invalid Tag[%s]"), *FString(__FUNCTION__), *Tag);
	}
}

void UExGameFeaturesCheat::DeactivateActions(const FString& Tag)
{
	FGameplayTag ModuleTag = UExGameplayLibrary::RequestGameplayTag(FName(Tag), false);
	if (ModuleTag.IsValid())
	{
		UExGameFeaturesSubsystem::GetSubsystem(this)->DeactivateModule(ModuleTag);
	}
	else
	{
		EXIGAMEFEATURE_LOG(Error, TEXT("%s error, Invalid Tag[%s]"), *FString(__FUNCTION__), *Tag);
	}
}
