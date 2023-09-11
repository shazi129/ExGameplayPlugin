#pragma once

#include "Components/ProjectilePredictLineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"
#include "ProjectileActor.generated.h"

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API AProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	AProjectileActor(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
		void Launch(FVector Velocity);

	UFUNCTION(BlueprintCallable)
		void SetCollisionEnabled(ECollisionEnabled::Type EnableType);

	UFUNCTION()
		void NativeOnProjectileStop(const FHitResult& ImpactResult);

	UFUNCTION(BlueprintImplementableEvent)
		void OnProjectileStop(const FHitResult& ImpactResult);

	void OnLaunchSuccess();

	UFUNCTION(BlueprintCallable)
	void ActivateProjectile();

	UFUNCTION(BlueprintCallable)
	void ClearTimeHandle();

	FORCEINLINE UProjectilePredictLineComponent* GetPredictLineComponent() const { return PredictLineComponent; }
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

public:
	//发射后多久开始模拟
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	float SimulatorDelay = 0.1f;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UProjectilePredictLineComponent> PredictLineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<USphereComponent> RootSphereCollision;

	FTimerHandle TimerHandle;
	FTimerHandle ExplosionTimeoutHandle;
};