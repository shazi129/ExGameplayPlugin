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
		static bool HasPawnStateTag(AActor* Actor, FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintPure)
		static bool CanEnterPawnStateAsset(AActor* Actor, const UPawnStateAsset* Asset);

	UFUNCTION(BlueprintPure)
		static bool CanEnterPawnState(AActor* Actor, const FGameplayTag& PawnStateTag);

	UFUNCTION(BlueprintCallable)
		static int32 EnterPawnState(AActor* Actor, const FGameplayTag& PawnStateTag, UObject* SourceObject=nullptr, UObject* Instigator=nullptr);

	UFUNCTION(BlueprintCallable)
		static bool LeavePawnState(AActor* Actor, int32 InstanceID, UObject* Instigator = nullptr);

	UFUNCTION(BlueprintPure)
		static UPawnStateEvent* GetEnterEventByTag(AActor* Actor, FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintPure)
		static UPawnStateEvent* GetLeaveEventByTag(AActor* Actor, FGameplayTag PawnStateTag);
};
