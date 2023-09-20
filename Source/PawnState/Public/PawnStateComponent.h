#pragma once 

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "AbilitySystemComponent.h"
#include "PawnStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnStateDelegate, const FPawnStateInstance&, PawnStateInstance);

UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateEvent : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPawnStateDelegate Delegate;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PAWNSTATE_API UPawnStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintPure)
		FString ToString();

	virtual bool CanEnterPawnState(const FGameplayTag& PawnState, FString& ErrMsg);

	UFUNCTION(BlueprintCallable)
		bool TryEnterPawnStateByAssets(const TArray<UPawnStateAsset*>& Assets, UPARAM(ref) TArray<int32>& Handles, UObject* SourceObject = nullptr, UObject* Instigator = nullptr);

	UFUNCTION(BlueprintCallable)
		bool LeavePawnState(int InstID, UObject* Instigator=nullptr);

	UFUNCTION(BlueprintCallable)
		bool LeaveAllPawnStateTag(FGameplayTag PawnState, UObject* Instigator = nullptr);

	UFUNCTION(BlueprintCallable)
		bool HasPawnStateTag(FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintCallable)
		UPawnStateEvent* GetEnterEventByTag(FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintCallable)
		UPawnStateEvent* GetLeaveEventByTag(FGameplayTag PawnStateTag);

	const TArray<FPawnStateInstance>& GetPawnStateInstances();

public:
	void RebuildCurrentTag();
	virtual int InternalEnterPawnState(const FGameplayTag& NewPawnStateTag, UObject* SourceObject = nullptr, UObject* Instigator = nullptr);
	virtual int InternalEnterPawnState(const FPawnState& PawnState, UObject* SourceObject = nullptr, UObject* Instigator = nullptr);
	virtual bool InternalLeavePawnState(const FPawnStateInstance& PawnStateInstance, UObject* Instigator=nullptr);

private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateEvent*> PawnStateEnterEvent;

	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateEvent*> PawnStateLeaveEvent;

	//角色身上是否挂了ASC， 如果挂了，得考虑ASC的逻辑
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	//本地记录的PawnState信息
	TArray<FPawnStateInstance> PawnStateInstances;
	FGameplayTagContainer CurrentPawnStateTags;

	static std::atomic<int32> InstanceIDGenerator;
};