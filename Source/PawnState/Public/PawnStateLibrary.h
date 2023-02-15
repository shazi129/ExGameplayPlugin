#pragma once

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "PawnStateComponent.h"
#include "PawnStateLibrary.generated.h"

UCLASS()
class PAWNSTATE_API UPawnStateLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UPawnStateComponent* GetLocalPawnStateComponent(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
		static class UPawnStateComponent* GetPawnStateComponent(AActor* Actor);

	UFUNCTION(BlueprintPure)
		static bool CanEnterPawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance);

	UFUNCTION(BlueprintCallable)
		static bool EnterPawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance);

	UFUNCTION(BlueprintCallable)
		static bool LeavePawnState(AActor* Actor, const FPawnStateInstance& PawnStateInstance);

	UFUNCTION(BlueprintPure)
		static UPawnStateEvent* GetEnterEventByTag(AActor* Actor, FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintPure)
		static UPawnStateEvent* GetLeaveEventByTag(AActor* Actor, FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintPure)
		static FPawnStateInstance GetGlobalPawnStateInstance(FName PawnStateTagName, UObject* SourceObject);

};
