#pragma once 

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "PawnStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnStateDelegate, const FPawnStateInstance&, PawnStateInstance);

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UPawnStateEvent : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPawnStateDelegate Delegate;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UPawnStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
		UPawnStateEvent* GetEnterEventByTag(FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintCallable)
		UPawnStateEvent* GetLeaveEventByTag(FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintCallable)
		UPawnStateEvent* GetEnterEvent(const UPawnStateAsset* PawnStateAsset);

	UFUNCTION(BlueprintCallable)
		UPawnStateEvent* GetLeaveEvent(const UPawnStateAsset* PawnStateAsset);

	UPROPERTY(BlueprintAssignable)
		FPawnStateDelegate ChangeDelegate;

	const TArray<FPawnStateInstance>& GetPawnStateInstances();

private:
	void RebuildCurrentTag();
	bool InternalCanEnterPawnState(const FPawnStateInstance& PawnStateInstance, FString* ErrMsg);
	bool InternalLeavePawnState(const FPawnStateInstance& PawnStateInstance, UObject* Instigator=nullptr);

private:
	TArray<FPawnStateInstance> PawnStateInstances;
	FGameplayTagContainer CurrentPawnStateTags;

	TMap<FGameplayTag, UPawnStateEvent*> PawnStateEnterEvent;
	TMap<FGameplayTag, UPawnStateEvent*> PawnStateLeaveEvent;
};