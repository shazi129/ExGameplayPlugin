#include "Components/ProjectilePredictLineComponent.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GameFramework/Character.h"
#include "UObject/NoExportTypes.h"
#include "ExGameplayPluginModule.h"

UProjectilePredictLineComponent::UProjectilePredictLineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UProjectilePredictLineComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UProjectilePredictLineComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}

void UProjectilePredictLineComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetEmitParamByViewport();
	UpdatePredictLine();
}

void UProjectilePredictLineComponent::SetEmitParamByViewport()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		float VelocityValue = DefaultVelocityValue > 0 ? DefaultVelocityValue : 500;

		AActor* ViewActor = PlayerController->PlayerCameraManager;

		//摄像机的正前方
		FVector RightVector = ViewActor->GetActorRightVector();

		FRotator Direction = ViewActor->GetActorForwardVector().Rotation();
		Direction += EmitViewOffset;

		StartVelocity = Direction.Vector() * VelocityValue;
		StartPosition = GetOwner()->GetActorLocation();
	}
}

FVector UProjectilePredictLineComponent::GetStartVelocity()
{
	return StartVelocity;
}

void UProjectilePredictLineComponent::SetPredictLineVisible(bool Visible)
{
	SetComponentTickEnabled(Visible);

	if (!Visible)
	{
		ClearSplinePoints(true);
		ResetSplineMeshes();
	}
}

void UProjectilePredictLineComponent::SetIgnoreActors(TArray<AActor*> InActorsToIgnore)
{
	ActorsToIgnore.Reset();
	ActorsToIgnore.Append(InActorsToIgnore);
}

void UProjectilePredictLineComponent::UpdatePredictLine()
{
	ClearSplinePoints(true);
	ResetSplineMeshes();

 	FPredictProjectilePathParams Params = FPredictProjectilePathParams(GrenadeRadius, StartPosition, StartVelocity, MaxSimulaTionTime);
	Params.bTraceWithCollision = TracePath;
	Params.bTraceComplex = TraceComplex;
	Params.ActorsToIgnore = ActorsToIgnore;
	Params.DrawDebugType = DrawDebugType;
	Params.DrawDebugTime = DrawDebugTime;
	Params.SimFrequency = SimFrequency;
	Params.OverrideGravityZ = OverrideGravityZ;
	Params.TraceChannel = TraceChannel; // Trace by channel

	// Do the trace
	FPredictProjectilePathResult PredictResult;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, PredictResult);

	if (PredictResult.PathData.Num() > 0)
	{
		for (int i = 0; i < PredictResult.PathData.Num(); i++)
		{
			AddSplinePoint(PredictResult.PathData[i].Location, ESplineCoordinateSpace::World, false);
			SetSplinePointType(i, ESplinePointType::Linear, true);
		}
		UpdateSpline();

		TArray<USplineMeshComponent*> MeshComponents;
		GetSplineMeshes(PredictResult.PathData.Num() - 1, MeshComponents);

		FVector StartLocation;
		FVector StartTangent;
		FVector EndLocation;
		FVector EndTangent;
		for (int i = 0; i < MeshComponents.Num(); i++)
		{
			GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent, ESplineCoordinateSpace::Local);
			GetLocationAndTangentAtSplinePoint(i+1, EndLocation, EndTangent, ESplineCoordinateSpace::Local);
			MeshComponents[i]->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);
		}

		/*
		

		if (ParamterCollection)
		{
			UKismetMaterialLibrary::SetVectorParameterValue(this, ParamterCollection, FName("StartPoint"), FLinearColor(PredictResult.PathData[0].Location));
			UKismetMaterialLibrary::SetVectorParameterValue(this, ParamterCollection, FName("EndPoint"), FLinearColor(PredictResult.PathData.Last().Location));
			UKismetMaterialLibrary::SetScalarParameterValue(this, ParamterCollection, FName("EndFalloff"), 0.0f);
		}
		*/
	}


}

void UProjectilePredictLineComponent::GetSplineMeshes(int Num, TArray<USplineMeshComponent*>& OutSplineMeshes)
{
	OutSplineMeshes.Reset();
	int NewMeshNum = Num - SplineMeshes.Num();
	if (NewMeshNum > 0)
	{
		if (SplineMeshClass == nullptr)
		{
			EXGAMEPLAY_LOG(Error, TEXT("%s error, SplineMeshClass is null"), *FString(__FUNCTION__));
			return;
		}

		FTransform DefaultTransform;
		for (int i = 0; i < NewMeshNum; i++)
		{
			USplineMeshComponent* MeshComponent = Cast<USplineMeshComponent>(GetOwner()->AddComponentByClass(SplineMeshClass, false, DefaultTransform, false));
			SplineMeshes.Add(MeshComponent);
		}
	}

	for (int i = 0; i < Num; i++)
	{
		SplineMeshes[i]->SetVisibility(true);
		OutSplineMeshes.Add(SplineMeshes[i]);
	}
	
}

void UProjectilePredictLineComponent::ResetSplineMeshes()
{
	for (USplineMeshComponent* Component : SplineMeshes)
	{
		//Component->SplineParams = FSplineMeshParams();
		Component->SetVisibility(false);
	}
}
