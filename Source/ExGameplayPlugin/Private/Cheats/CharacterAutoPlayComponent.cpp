#include "Cheats/CharacterAutoPlayComponent.h"
#include "GameFramework/Character.h"
#include "ExGameplayPluginModule.h"

UCharacterAutoPlayComponent::UCharacterAutoPlayComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterAutoPlayComponent::BeginPlay()
{
	Super::BeginPlay();

    if (PlayInterval <= 0)
    {
        PlayInterval = 60;
    }
    CurrentInterval = PlayInterval;
    CurrentAction = 0;

    OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UCharacterAutoPlayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (CurrentInterval <= 0)
    {
        CurrentInterval = PlayInterval;
        UpdateCurrentState();
    }

    CurrentInterval--;
    HandleCurrentAction();
}

void UCharacterAutoPlayComponent::UpdateCurrentState()
{
    //更新目的地
    FVector2D TargetPoint2D = FMath::RandPointInCircle(MoveRange);
    TargetPosition = MoveCenter + FVector(TargetPoint2D.X, TargetPoint2D.Y, 0);

    //随机选一个动作
    CurrentAction = FMath::RandHelper(3);
    EXGAMEPLAY_LOG(Log, TEXT("%s CurrentAction：%d"), *FString(__FUNCTION__), CurrentAction);
}

void UCharacterAutoPlayComponent::HandleCurrentAction()
{
    if (OwnerCharacter == nullptr)
    {
        EXGAMEPLAY_LOG(Log, TEXT("%s error, Owner Character is null"), *FString(__FUNCTION__));
        return;
    }

    switch (CurrentAction)
    {
    case 0:   //保持不动
        break;
    case 1:
        HandleJump(); //跳
        break;
    case 2:
        HandleRun(); //跑
        break;
    default:
        EXGAMEPLAY_LOG(Log, TEXT("%s unresolved action: %D"), *FString(__FUNCTION__), CurrentAction);
    }
}

void UCharacterAutoPlayComponent::HandleJump()
{
    if (!OwnerCharacter)
    {
        EXGAMEPLAY_LOG(Error, TEXT("%s error, Owner character is null"), *FString(__FUNCTION__), CurrentAction);
        return;
    }

    OwnerCharacter->Jump();
}

void UCharacterAutoPlayComponent::HandleRun()
{
    if (!OwnerCharacter)
    {
        EXGAMEPLAY_LOG(Error, TEXT("%s error, Owner character is null"), *FString(__FUNCTION__), CurrentAction);
        return;
    }
    FVector CurrentLocation = OwnerCharacter->GetActorLocation();
    FVector TargetDirction = TargetPosition - CurrentLocation;
    TargetDirction.Normalize(0.001);
    OwnerCharacter->AddMovementInput(TargetDirction, 3);
}
