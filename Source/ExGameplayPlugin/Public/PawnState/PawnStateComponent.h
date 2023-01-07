#pragma once 

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "PawnStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnStateDelegate, const FPawnStateInstance&, PawnStateInstance);

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
	UFUNCTION(BlueprintPure)
		FString ToString();

	UFUNCTION(BlueprintCallable)
		bool CanEnterPawnState(const FPawnStateInstance& PawnStateInstance);

	UFUNCTION(BlueprintCallable)
		bool EnterPawnState(const FPawnStateInstance& PawnStateInstance);

	UFUNCTION(BlueprintCallable)
		bool LeavePawnState(const FPawnStateInstance& PawnStateInstance);

	UFUNCTION(BlueprintCallable)
		bool HasPawnState(const FPawnStateInstance& PawnStateInstance);

	UFUNCTION(BlueprintCallable)
		FPawnStateEvent& GetEnterEvent(const UPawnState* PawnState);

	UFUNCTION(BlueprintCallable)
		FPawnStateEvent& GetLeaveEvent(const UPawnState* PawnState);

private:
	void RebuildCurrentTag();
	bool InternalCanEnterPawnState(const FPawnStateInstance& PawnStateInstance, FString* ErrMsg);
	bool InternalLeavePawnState(const FPawnStateInstance& PawnStateInstance, UObject* Instigator=nullptr);

private:
	TArray<FPawnStateInstance> PawnStateInstances;
	FGameplayTagContainer CurrentPawnStateTags;

	TMap<const UPawnState*, FPawnStateEvent> PawnStateEnterEvent;
	TMap<const UPawnState*, FPawnStateEvent> PawnStateLeaveEvent;
};