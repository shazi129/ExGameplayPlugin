#pragma once

#include "CoreMinimal.h"
#include "CharacterAutoPlayComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UCharacterAutoPlayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UCharacterAutoPlayComponent();
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

private:
    void UpdateCurrentState();

    void HandleCurrentAction();
    void HandleJump();
    void HandleRun();

private:
    int CurrentInterval = 0;
    int CurrentAction = 0;

    class ACharacter* OwnerCharacter;
    FVector TargetPosition;
};