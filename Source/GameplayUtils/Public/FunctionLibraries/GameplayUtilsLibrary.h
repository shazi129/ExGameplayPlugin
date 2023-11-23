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

	UFUNCTION(BlueprintPure, Category = "GameplayUtils", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool CheckExecNetMode(const UObject* WorldContextObject, EExecNetMode ExecNetMode);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool IsLocalControled(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static UObject* GetDefaultObject(UClass* ObjectClass);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static APawn* GetPawnByPlayerState(APlayerState* PlayerState);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static APlayerState* GetPlayerStateByActor(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static UActorComponent* GetComponentByTag(AActor* Actor, const FName& Tag);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static FString GetNameSafe(const UObject* Object);
};