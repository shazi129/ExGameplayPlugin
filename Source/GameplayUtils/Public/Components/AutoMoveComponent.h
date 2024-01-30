#pragma once

#include "CoreMinimal.h"
#include "AutoMoveComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UAutoMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UAutoMoveComponent();
	virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    //活动中心点
    UPROPERTY(EditAnywhere)
        FVector MoveCenter;

    //活动范围
    UPROPERTY(EditAnywhere)
        float MoveRange;

    //决策行为间隔, 按帧算
    UPROPERTY(EditAnywhere)
        int PlayInterval;

 public:
    UFUNCTION(BlueprintImplementableEvent)
    bool HandleJump();

    UFUNCTION(BlueprintImplementableEvent)
    bool HandleRun();

    UFUNCTION(BlueprintImplementableEvent)
    bool HandleStay();

protected:
    void NativeHandleJump();
    void NativeHandleRun();
    void NativeHandleStay();

private:
    void UpdateCurrentState();

    void HandleCurrentAction();

private:
    int CurrentInterval = 0;
    int CurrentAction = 0;

    FVector TargetPosition;
};