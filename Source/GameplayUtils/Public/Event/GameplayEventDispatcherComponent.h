#pragma once

#include "GameplayTags.h"
#include "Components/ActorComponent.h"
#include "Event/GameplayEventWatcher.h"
#include "Event/GameplayEventHandler.h"
#include "GameplayEventDispatcherComponent.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FGameplayWaitEventItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced)
	UGameplayEventWatcher* Watcher;

	UPROPERTY(EditAnywhere, Instanced)
	UGameplayEventHandler* Handler;

	void Initialize(UObject* SourceObject);
	bool IsValid() const;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UGameplayEventDispatcherComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAnimation(USkeletalMeshComponent* SkeletalMeshComponent, UAnimationAsset* AnimationAsset, bool bLooping);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastExecuteHandler(TSubclassOf<UGameplayEventHandler> HandlerClass, const FGameplayEventContext& Context);

	void ExecuteHandler(TSubclassOf<UGameplayEventHandler> HandlerClass, const FGameplayEventContext& Context);
	void ExecuteHandler(const FGameplayTag& HandlerTag, const FGameplayEventContext& Context);

	UFUNCTION(BlueprintCallable)
	void ExecuteHandlerWithTagAndInstigator(FGameplayTag HandlerTag, UObject* Instigator);

public:
	//引用的Handler，用于解决可能存在循环引用的问题
	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<UGameplayEventHandler>> HandlerClasses;

	//实例化的Handler, 通用的一些Handler，通过参数不同实现不同的功能
	UPROPERTY(EditAnywhere, Instanced)
	TArray<UGameplayEventHandler*> InstancedHandlerList;

	//wather-handler
	UPROPERTY(EditAnywhere)
	TArray<FGameplayWaitEventItem> WaitEventList;

protected:
	//实例化的Handler
	UPROPERTY(Transient)
	TArray<UGameplayEventHandler*> HandlerList;
};