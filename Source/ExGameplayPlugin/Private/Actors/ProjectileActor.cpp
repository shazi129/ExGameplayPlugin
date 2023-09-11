#include "Actors/ProjectileActor.h"
#include "Kismet/GameplayStatics.h"
#include "ExGameplayPluginModule.h"

AProjectileActor::AProjectileActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = RootSphereCollision;

	PredictLineComponent = CreateDefaultSubobject<UProjectilePredictLineComponent>(TEXT("PredictLine"));
	PredictLineComponent->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->bSimulationEnabled = false;
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	//开始时不加物理
	ProjectileMovement->bSimulationEnabled = false;
	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement->OnProjectileStop.AddDynamic(this,&AProjectileActor::NativeOnProjectileStop);
}

void AProjectileActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ProjectileMovement->OnProjectileStop.RemoveDynamic(this, &AProjectileActor::NativeOnProjectileStop);
}

void AProjectileActor::Launch(FVector Velocity)
{
	PredictLineComponent->SetPredictLineVisible(false);

	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->bSimulationEnabled = true;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	//发射后开启碰撞
	if (SimulatorDelay > 0.0f)
	{
		TimerManager.SetTimer(TimerHandle, this, &AProjectileActor::OnLaunchSuccess, SimulatorDelay, false);
	}
}

void AProjectileActor::SetCollisionEnabled(ECollisionEnabled::Type EnableType)
{
	RootSphereCollision->SetCollisionEnabled(EnableType);
}


void AProjectileActor::NativeOnProjectileStop(const FHitResult& ImpactResult)
{
	ClearTimeHandle();
	OnProjectileStop(ImpactResult);
}

void AProjectileActor::OnLaunchSuccess()
{
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void AProjectileActor::ActivateProjectile()
{
	ProjectileMovement->SetUpdatedComponent(RootSphereCollision);
}

void AProjectileActor::ClearTimeHandle()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(ExplosionTimeoutHandle);
	TimerManager.ClearTimer(TimerHandle);
}