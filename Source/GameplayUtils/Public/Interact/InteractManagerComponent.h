#pragma once

#include "Actors/AbilityActor.h"
#include "Interact/InteractItemComponent.h"
#include "InteractManagerComponent.generated.h"

/**
 * 主要挂在本地角色身上，用于处理RPC相关， 如果一个交互系统不需要RPC， 则不需要挂这个组件
 */

UCLASS(ClassGroup = (Interact), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UInteractManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//角色自定义的交互范围
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FInteractRange InteractRange;

	//客户端向服务器请求的交互
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerRequestStartInteract(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName);

	//客户端向服务器请求的交互
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerRequestStopInteract(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastStartInteractItem(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastStopInteractItem(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName);
};
