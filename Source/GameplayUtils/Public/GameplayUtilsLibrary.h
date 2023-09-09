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
	
	//Actor中不能有这些Component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UActorComponent>> ExcludeComponentClasses;

	//Actor必须属于在指定的Class
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<AActor>> ActorClasses;

	//忽略的Actor
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> IgnoreActors;

	bool FilterActorClasses(AActor* Actor) const;
	bool FilterExcludeComponentClasses(AActor* Actor) const;
	bool FilterRequireComponentClasses(AActor* Actor) const;
	bool FilterIgnoreActors(AActor* Actor) const;
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