#pragma once

#include "CoreMinimal.h"
#include "Components/VAT/VATControlComponent.h"
#include "Components/VAT/VATDefaultAnimationTypes.h"
#include "GameFramework/NavMovementComponent.h"
#include "VATCharacterMovementComponent.generated.h"



UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UVATCharacterMovementComponent : public UNavMovementComponent
{
	GENERATED_BODY()

public:
	UVATCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReson) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	virtual bool IsCrouching() const override;
	virtual bool IsFalling() const override;
	virtual bool IsMovingOnGround() const override;
	virtual bool IsSwimming() const override;
	virtual bool IsFlying() const override;

	virtual void NativeUpdateAnimation(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAnimation(float InteractData);

	UFUNCTION(BlueprintPure)
	UVATControlComponent* GetVATControlComponent();

	UFUNCTION(BlueprintCallable)
	void PlayAnimation(int AnimationType);

	//默认的动画状态机
	UFUNCTION(BlueprintCallable)
	void UpdateDefaultAnimSM();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation);
	uint8 bDontBlend: 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation);
	TMap<TEnumAsByte<EVATAnimationType>, FAnimationPlayInfo> DefaultAnimationInfo;

public:
	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<enum EMovementMode> MovementMode;

	/** When true, player wants to jump */
	UPROPERTY(BlueprintReadOnly)
		uint32 bPressedJump : 1;

	/** Set by character movement to specify that this Character is currently crouched. */
	UPROPERTY(BlueprintReadOnly)
		uint32 bIsCrouched : 1;

	UPROPERTY(BlueprintReadOnly)
		float Speed;

private:
	//动画控制器
	UPROPERTY()
	UVATControlComponent* VATController;

	//动画状态机相关
	double CurrentStateStartTime;
	EVATAnimationType CurrentAnimState;
	EVATAnimationType OldAnimState;
	void TransactionState(bool Transaction, EVATAnimationType TargetState, EVATAnimationType OtherState = EVATAnimationType::E_NONE);

	//所有可支持的动画
	UPROPERTY()
	TMap<int, FAnimationPlayInfo> AnimationInfoMap;
};