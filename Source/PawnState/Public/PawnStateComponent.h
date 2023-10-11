#pragma once 

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "InstancedStruct.h"
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
		bool HasPawnStateTag(FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintCallable)
		UPawnStateEvent* GetEnterEventByTag(FGameplayTag PawnStateTag);

	UFUNCTION(BlueprintCallable)
		UPawnStateEvent* GetLeaveEventByTag(FGameplayTag PawnStateTag);

	const TArray<FPawnStateInstance>& GetPawnStateInstances();

	FPawnStateInstance* FindPawnStateInstance(const FGameplayTag& NewPawnStateTag, UObject* SourceObject, UObject* Instigator);

public:
	void RebuildCurrentTag();
	virtual int InternalEnterPawnState(const FGameplayTag& NewPawnStateTag, UObject* SourceObject = nullptr, UObject* Instigator = nullptr);
	virtual bool InternalLeavePawnState(int InstanceID, UObject* Instigator=nullptr);
	void HandleStateEvent(EPawnStateEventTriggerType TriggerType, const FPawnStateInstance& Instance);

private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateEvent*> PawnStateEnterEvent;

	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateEvent*> PawnStateLeaveEvent;

	//本地记录的PawnState信息
	TArray<FPawnStateInstance> PawnStateInstances;

	//用于复制到模拟端的结构
	UPROPERTY(Replicated)
	FGameplayTagContainer CurrentPawnStateTags;

	//StateID 生成器
	static std::atomic<int32> InstanceIDGenerator;


/////////////////////////////Cheat相关///////////////////
public:
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void SendMsgToServer(const FGameplayTag& MsgTag, const FInstancedStruct& MsgBody);

	UFUNCTION(Reliable, Client, WithValidation, BlueprintCallable)
	void SendMsgToClient(const FGameplayTag& MsgTag, const FInstancedStruct& MsgBody);
};