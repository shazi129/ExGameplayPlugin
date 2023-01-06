#pragma once 

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "PawnStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnStateDelegate, const FGameplayTag&, PawnState);

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FPawnStateEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FPawnStateDelegate Delegate;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UPawnStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool CanEnterState(const FGameplayTag& NewPawnState);

	UFUNCTION(BlueprintCallable)
	bool EnterState(const FGameplayTag& NewPawnState);

	UFUNCTION(BlueprintCallable)
	bool LeaveState(const FGameplayTag& PawnState);

	UFUNCTION(BlueprintCallable)
	bool HasState(const FGameplayTag& PawnState, bool Exactly=true);

	UFUNCTION(BlueprintCallable)
		FPawnStateEvent& GetEnterEvent(const FGameplayTag& PawnState);

	UFUNCTION(BlueprintCallable)
		FPawnStateEvent& GetLeaveEvent(const FGameplayTag& PawnState);

	UFUNCTION(BlueprintPure)
		FString ToString();

private:
	FGameplayTagContainer CurrentPawnStates;

	TArray<FPawnStateInstance> PawnStateInstances;

	TMap<FGameplayTag, FPawnStateEvent> PawnStateEnterEvent;
	TMap<FGameplayTag, FPawnStateEvent> PawnStateLeaveEvent;
};