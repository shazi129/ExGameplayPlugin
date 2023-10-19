#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "GameplayTypes.h"
#include "GameplayUtilsLibrary.generated.h"

UCLASS()
class GAMEPLAYUTILS_API UGameplayUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static bool ExecCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static void FilterActors(const TArray<AActor*>& Actors, const FFilterActorCondition FilterCondition, TArray<AActor*>& OutActors);

	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static bool CheckExecNetMode(const UObject* WorldContextObject, EExecNetMode ExecNetMode);
};