#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GameplayTags.h"
#include "ExGameFeaturesCheat.generated.h"

UCLASS()
class EXGAMEFEATURES_API UExGameFeaturesCheat : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
		void ExGF(const FString& Param);

private:
	FGameplayTag ParseParamTagValue(const FString& Param, const FString& Name);

	void ShowModularActions();

	void ShowUsage();

	void ActivateActions(const FString& Tag);
	void DeactivateActions(const FString& Tag);
};