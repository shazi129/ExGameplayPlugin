// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interact/InteractItemComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "InteractSubsystem.generated.h"

/**
 * 收集及处理交互
 */

UCLASS()
class GAMEPLAYUTILS_API UInteractSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UInteractSubsystem* Get(const UObject* WorldContextObject);

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	//注册可交互组件信息
	UFUNCTION(BlueprintCallable)
	void RegisterInteractItem(TScriptInterface<IInteractItemInterface> IInteractItem, AActor* ItemActor);

	UFUNCTION(BlueprintCallable)
	void UnregisterInteractItem(TScriptInterface<IInteractItemInterface> IInteractItem);

	//设置交互角色
	UFUNCTION(BlueprintCallable)
	void SetInteractPawn(APawn* Pawn);

	UFUNCTION(BlueprintCallable)
	bool AddInteracts(TScriptInterface<IInteractItemInterface> InteractItem, AActor* ItemActor);

	bool CanInteract(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName);

	void InternalStartInteractItem(TScriptInterface<IInteractItemInterface> InteractItem, APawn* Instigator, const FName& ConfigName);

	void InternalStopInteractItem(TScriptInterface<IInteractItemInterface> InteractItem, APawn* Instigator, const FName& ConfigName);

	//整个系统是否可用
	UFUNCTION(BlueprintCallable)
	void SetEnable(bool Enable);

	UFUNCTION(BlueprintCallable)
	bool GetEnable();

	//某个响应项是否可用
	UFUNCTION(BlueprintCallable)
	void SetItemEnable(TScriptInterface<IInteractItemInterface> ItemInterface, const FName& ConfigName, bool Enable);

	UFUNCTION(BlueprintCallable)
	bool GetItemEnable(TScriptInterface<IInteractItemInterface> ItemInterface, const FName& ConfigName);

	//某个component的所有响应项是否可用
	UFUNCTION(BlueprintCallable)
	void SetItemEnableByInterface(TScriptInterface<IInteractItemInterface> ItemInterface, bool Enable);

	UFUNCTION(BlueprintCallable)
	void ReceiveInput(const FGameplayTag& InputTag);

	void UpdateItem(FInteractInstanceData& IteractData);

	//对可响应的列表进行排序，选出最高优先级的物体
	void SortItems();

private:
	UFUNCTION()
	void Tick(float Seconds);

	//根据Component和配置名找数据
	FInteractInstanceData* FindInstance(UObject* ItemObject, const FName& ConfigName);

	virtual void RebuildInteractData();

	void StartInteractItem(FInteractInstanceData* InstanceData);
	void StopInteractItem(FInteractInstanceData* InstanceData);

private:
	

	FName TickName;

	void IgnoreVectorAxis(FVector& Vector, EInteractAngleAxis IgnoreAxis);
	FInteractInstanceData* GetInteractingInstance();
	void SetInteracting(FInteractInstanceData* InstanceData);
	

	//本地交互对象，通常是玩家控制的角色
	TWeakObjectPtr<APawn> InteractPawn;

	//玩家是否在交互中, 只在Client端有用
	TWeakObjectPtr<UObject> InteractingObject;
	FName InteractingConfig;

	uint8 bEnabled: 1;

	//玩家是否在交互中, 只在Client端有用
	uint8 bInServer: 1;

	//玩家是否在交互中, 只在Client端有用
	uint8 bDataDirty : 1;

	//玩家本身对交互的配置，例如一个角色只能对它身前的物体交互
	FInteractRange PawnInteractRange;

	//收集到的状态为可交互的物体
	TArray<FInteractInstanceData*> InteractiveInstanceList;

	//收集到状态不为None的信息
	TArray<FInteractInstanceData*> ActivateInstanceList;

	//收集到需要通知的交互信息
	TArray<FInteractInstanceData*> NotifyInstanceList;

	//当前所有的的可交互物体
	UPROPERTY()
	TMap<TWeakObjectPtr<UObject>, FObjectInstanceDataInfo> InteractInstanceMap;

	//当前需要删除的可交互物体
	TArray<TWeakObjectPtr<UObject>> InvalidateObjects;
};
