#pragma once 

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "InstancedStruct.h"
#include "Debug/DebugDisplay.h"
#include "PawnStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnStateDelegate, const FPawnStateInstance&, PawnStateInstance);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPawnStateEventDelegate, EPawnStateEventType, EventType, const FPawnStateInstance&, Instance);

UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateEvent : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPawnStateDelegate Delegate;
};

UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateEventWrapper : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPawnStateEventDelegate Delegate;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PAWNSTATE_API UPawnStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	static void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);

public:
	UFUNCTION(BlueprintPure)
		FString ToString();

	/**
	 * @brief 是否能进入某个State
	 * @param PawnState  要进入的State
	 * @param ErrMsg  失败的原因
	 * @return 是否成功
	*/
	virtual bool CanEnterPawnState(const FGameplayTag& PawnState, FString& ErrMsg);
	virtual bool CanEnterPawnStates(const FGameplayTagContainer& PawnStates, FString& ErrMsg);

	/**
	 * @brief 尝试进入某个State， 使用场景：A扔了个炸弹，导致B进入掉血状态
	 * @param PawnStateTag 要进入的State， 掉血状态
	 * @param SourceObject 导致进入的Object， 炸弹
	 * @param Instigator 本次行为发起者， A
	 * @return PawnState ID
	*/
	UFUNCTION(BlueprintCallable)
		int TryEnterPawnState(FGameplayTag PawnStateTag, UObject* SourceObject = nullptr, UObject* Instigator = nullptr);

	UFUNCTION(BlueprintCallable)
		bool TryEnterPawnStateByAssets(const TArray<UPawnStateAsset*>& Assets, UPARAM(ref) TArray<int32>& Handles, UObject* SourceObject = nullptr, UObject* Instigator = nullptr);

	UFUNCTION(BlueprintCallable)
		bool TryEnterPawnStateByTags(const TArray<FGameplayTag>& PawnStateTags, UPARAM(ref) TArray<int32>& Handles, UObject* SourceObject = nullptr, UObject* Instigator = nullptr);
	
	/**
	 * @brief 离开某个State
	 * @param InstID  该State的ID
	 * @param Instigator  离开发起者
	 * @return 是否成功
	*/
	UFUNCTION(BlueprintCallable)
		bool LeavePawnState(int InstID, UObject* Instigator=nullptr);

	UFUNCTION(BlueprintCallable)
		bool LeaveAllPawnStateTag(FGameplayTag PawnState, UObject* Instigator = nullptr);

	UFUNCTION(BlueprintCallable)
		bool HasPawnStateTag(FGameplayTag PawnStateTag, bool OnlyLocal=false);

	bool HasPawnStateAsset(TSoftObjectPtr<UPawnStateAsset>& PawnState);

	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction, DeprecationMessage = "Please use GetEventByTag"))
		UPawnStateEvent* GetEnterEventByTag(FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintCallable, meta=(DeprecatedFunction, DeprecationMessage="Please use GetEventByTag"))
		UPawnStateEvent* GetLeaveEventByTag(FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintCallable)
		UPawnStateEventWrapper* GetEventByTag(FGameplayTag PawnStateTag);

	const TArray<FPawnStateInstance>& GetPawnStateInstances();

	FPawnStateInstance* FindPawnStateInstance(const FGameplayTag& NewPawnStateTag, UObject* SourceObject, UObject* Instigator);

	FPawnStateEventDelegate& OnPawnStateEventDelegate()
	{
		return PawnStateEventDelegate;
	}

public:
	virtual int InternalEnterPawnState(const FGameplayTag& NewPawnStateTag, UObject* SourceObject = nullptr, UObject* Instigator = nullptr);
	virtual bool InternalLeavePawnState(int InstanceID, UObject* Instigator=nullptr);
	
private:
	void RebuildCurrentTag();
	void HandleStateEvent(EPawnStateEventType TriggerType, const FPawnStateInstance& Instance);

	UFUNCTION()
	virtual void OnRep_PawnStateTags(const FGameplayTagContainer& OldValue);

public:
	UPROPERTY(BlueprintAssignable)
	FPawnStateEventDelegate PawnStateEventDelegate;

private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateEvent*> PawnStateEnterEvent;

	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateEvent*> PawnStateLeaveEvent;

	//动态设置的Event
	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateEventWrapper*> PawnStateEventMap;

	//Asset中配置的Event， 这里复制一遍，防止操作到公用资源
	TMap<FGameplayTag, TArray<FInstancedStruct>> PawnStateInstancedEventMap;

	//本地记录的PawnState信息
	TArray<FPawnStateInstance> PawnStateInstances;
	FGameplayTagContainer CurrentPawnStateTags;

	//用于复制到模拟端的结构
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_PawnStateTags)
	FGameplayTagContainer ReplicatePawnStateTags;

	//StateID 生成器
	static std::atomic<int32> InstanceIDGenerator;

	FDebugDisplayProxy DebugDisplayProxy;
	void CollectDebugContent();

/////////////////////////////Cheat相关///////////////////
public:
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void SendMsgToServer(const FGameplayTag& MsgTag, const FInstancedStruct& MsgBody);

	UFUNCTION(Reliable, Client, WithValidation, BlueprintCallable)
	void SendMsgToClient(const FGameplayTag& MsgTag, const FInstancedStruct& MsgBody);
};