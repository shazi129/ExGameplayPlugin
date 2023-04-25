#include "Components/VAT/VATCharacterMovementComponent.h"
#include "ExGameplayPluginModule.h"

UVATCharacterMovementComponent::UVATCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
}

void UVATCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	//填充配置数据
	for (auto DefaultAnimationInfoItem : DefaultAnimationInfo)
	{
		AnimationInfoMap.Add(DefaultAnimationInfoItem.Key, DefaultAnimationInfoItem.Value);
	}
}

void UVATCharacterMovementComponent::EndPlay(EEndPlayReason::Type EndPlayReson)
{
	Super::EndPlay(EndPlayReson);
}

void UVATCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	NativeUpdateAnimation(DeltaTime);

	//更新动画状态机
	UpdateDefaultAnimSM();
}

void UVATCharacterMovementComponent::NativeUpdateAnimation(float DeltaTime)
{
	Speed = Velocity.Size();
	UpdateAnimation(DeltaTime);
}

UVATControlComponent* UVATCharacterMovementComponent::GetVATControlComponent()
{
	if (!VATController)
	{
		AActor* Owner = GetOwner();
		VATController = Cast<UVATControlComponent>(Owner->GetComponentByClass(UVATControlComponent::StaticClass()));
		if (!VATController)
		{
			EXGAMEPLAY_LOG(Error, TEXT("%s: Actor[%s] has no component of type UVATControlComponent"), *FString(__FUNCTION__), *GetNameSafe(Owner));
		}
	}

	return VATController;
}

void UVATCharacterMovementComponent::PlayAnimation(int AnimationType)
{
	if (UVATControlComponent* Controller = GetVATControlComponent())
	{
		if (const FAnimationPlayInfo* AnimationInfo = AnimationInfoMap.Find(AnimationType))
		{
			Controller->PlayAnimation(AnimationInfo->AnimationName, bDontBlend ? 0.0f : AnimationInfo->BlendTime);
		}
		else
		{
			EXGAMEPLAY_LOG(Error, TEXT("%s: Cannot find VAT Animation by type %d"), *FString(__FUNCTION__), AnimationType);
		}
	}
	else
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s: Cannot find VAT control component in %s"), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
	}
}

bool UVATCharacterMovementComponent::IsMovingOnGround() const
{
	return ((MovementMode == MOVE_Walking) || (MovementMode == MOVE_NavWalking)) && UpdatedComponent;
}

bool UVATCharacterMovementComponent::IsFalling() const
{
	return (MovementMode == MOVE_Falling) && UpdatedComponent;
}

bool UVATCharacterMovementComponent::IsSwimming() const
{
	return (MovementMode == MOVE_Swimming) && UpdatedComponent;
}

bool UVATCharacterMovementComponent::IsCrouching() const
{
	return bIsCrouched;
}

bool UVATCharacterMovementComponent::IsFlying() const
{
	return (MovementMode == MOVE_Flying) && UpdatedComponent;
}

void UVATCharacterMovementComponent::TransactionState(bool Transaction, EVATAnimationType TargetState, EVATAnimationType OtherState)
{
	if (Transaction)
	{
		CurrentAnimState = TargetState;
	}
	else if (OtherState != EVATAnimationType::E_NONE)
	{
		CurrentAnimState = OtherState;
	}
}

void UVATCharacterMovementComponent::UpdateDefaultAnimSM()
{
	//第一次进入状态机时的初始化
	if (CurrentAnimState == EVATAnimationType::E_NONE)
	{
		OldAnimState = EVATAnimationType::E_NONE;
		CurrentAnimState = EVATAnimationType::E_Idle;
		CurrentStateStartTime = FPlatformTime::Seconds();
	}

	//是否在地面
	bool InGround = CurrentAnimState < EVATAnimationType::E_Jump;
	
	if (InGround)
	{
		if (MovementMode == MOVE_Falling)
		{
			CurrentAnimState = Velocity.Z > 100? E_Jump : E_FallLoop;
		}
		else
		{
			switch (CurrentAnimState)
			{
			case E_Idle:
				{
					TransactionState(Speed > 0, EVATAnimationType::E_RunFwd);
				}
				break;
			case E_WalkInPace:
				break;
			case E_WalkFwd:
				break;
			case E_RunFwd:
				{
					TransactionState(Speed == 0, EVATAnimationType::E_Idle);
				}
				break;
			default:
				break;
			}
		}	
	}
	else
	{
		switch (CurrentAnimState)
		{
		case E_Jump:
			{
				TransactionState(FPlatformTime::Seconds() - CurrentStateStartTime > 0.1f, EVATAnimationType::E_FallLoop);
			}
			break;
		case E_FallLoop:
			{
				if (Velocity.Z == 0)
				{
					CurrentAnimState = E_Land;
				}
			}
			break;
		case E_Land:
			{
				TransactionState(FPlatformTime::Seconds() - CurrentStateStartTime > 0.1f, EVATAnimationType::E_Idle);
			}
			break;
		default:
			break;
		}
	}

	if (OldAnimState != CurrentAnimState)
	{
		PlayAnimation(CurrentAnimState);
		CurrentStateStartTime = FPlatformTime::Seconds();
	}
	OldAnimState = CurrentAnimState;
}
