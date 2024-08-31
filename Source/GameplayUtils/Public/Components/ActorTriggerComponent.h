#pragma once

/**
 * 
 * Actor级别的检测组件
 * 使用场景：
 * 1. Actor上挂有多个可被检测的Component，一个被检测到就算actor被检测到了。
 * 2. 有个检测器带有多个碰撞体，一个碰撞体overlap到了目标actor，就算检测成功了
 * 
 */

#include "GameplayTypes.h"
#include "Components/ActorComponent.h"
#include "ActorTriggerComponent.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FTriggeredActorInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor;

	TArray<TWeakObjectPtr<UPrimitiveComponent>> Components;
};

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UActorTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> TiggerShapeNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSoftClassPtr<AActor>> ActorClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSoftClassPtr<UPrimitiveComponent>> ComponentClasses;

protected:
	UFUNCTION()
	virtual void NativeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void NativeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTriggerBeginOverlap"))
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTriggerEndOverlap"))
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual bool IsTargetActor(AActor* OtherActor, UPrimitiveComponent* OtherComp);

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FTriggeredActorInfo> OverlappingInfoList;
};