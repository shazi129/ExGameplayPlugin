#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "GameplayUtilsLibrary.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FFilterActorCondition
{
	GENERATED_BODY()

	//Actor中必须有这些Component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UActorComponent>> RequireComponentClasses;
	
	//Actor必须属于在指定的Class
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<AActor>> ActorClasses;
};

UCLASS()
class GAMEPLAYUTILS_API UGameplayUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static bool ExecCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static void FilterActors(const TArray<AActor*>& Actors, const FFilterActorCondition FilterCondition, TArray<AActor*>& OutActors);
};