#include "Components/AutoMoveComponent.h"
#include "GameplayUtilsModule.h"
#include "GameFramework/Character.h"
UAutoMoveComponent::UAutoMoveComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAutoMoveComponent::BeginPlay()
{
	Super::BeginPlay();

    if (PlayInterval <= 0)
    {
        PlayInterval = 60;
    }
    CurrentInterval = PlayInterval;
    CurrentAction = 0;
}

void UAutoMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (CurrentInterval <= 0)
    {
        CurrentInterval = PlayInterval;
        UpdateCurrentState();
    }

    CurrentInterval--;
    HandleCurrentAction();
}

void UAutoMoveComponent::UpdateCurrentState()
{
    //更新目的地
    FVector2D TargetPoint2D = FMath::RandPointInCircle(MoveRange);
    TargetPosition = MoveCenter + FVector(TargetPoint2D.X, TargetPoint2D.Y, 0);

    //随机选一个动作
    CurrentAction = FMath::RandHelper(3);
    //GAMEPLAYUTILS_LOG(Log, TEXT("%s CurrentAction：%d"), *FString(__FUNCTION__), CurrentAction);
}

void UAutoMoveComponent::HandleCurrentAction()
{
    switch (CurrentAction)
    {
    case 0:   //保持不动
        NativeHandleStay();
        break;
    case 1:
        NativeHandleJump(); //跳
        break;
    case 2:
        NativeHandleRun(); //跑
        break;
    default:
        break;
    }
}

void UAutoMoveComponent::NativeHandleJump()
{
    if (HandleJump())
    {
        return;
    }
    if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        OwnerCharacter->Jump();
    }
}

void UAutoMoveComponent::NativeHandleRun()
{
    if (HandleRun())
    {
        return;
    }
    if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    {
        FVector CurrentLocation = OwnerPawn->GetActorLocation();
        FVector TargetDirction = TargetPosition - CurrentLocation;
        TargetDirction.Normalize(0.001);
        OwnerPawn->AddMovementInput(TargetDirction, 3);
    }
}

void UAutoMoveComponent::NativeHandleStay()
{
    if (HandleStay())
    {
        return;
    }
}
