#include "Components/ProjectilePredictLineComponent.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "UObject/NoExportTypes.h"

void UProjectilePredictLineComponent::SetEmitParam(FVector InStartPosition, FVector InStartVelocity)
{
	StartPosition = InStartPosition;
	StartVelocity = InStartVelocity;
}


void UProjectilePredictLineComponent::SetIgnoreActors(TArray<AActor*> InActorsToIgnore)
{
	ActorsToIgnore.Reset();
	ActorsToIgnore.Append(InActorsToIgnore);
}

void UProjectilePredictLineComponent::UpdatePredictLine()
{
	ClearSplinePoints(true);

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

	if (PredictResult.PathData.Num() > 0 && ParamterCollection)
	{
		for (const FPredictProjectilePathPointData& PathPoint : PredictResult.PathData)
		{
			AddSplinePoint(PathPoint.Location, ESplineCoordinateSpace::World, false);
		}

		SetSplinePointType(PredictResult.PathData.Num() - 1, ESplinePointType::Linear, true);

		UKismetMaterialLibrary::SetVectorParameterValue(this, ParamterCollection, FName("StartPoint"), FLinearColor(PredictResult.PathData[0].Location));
		UKismetMaterialLibrary::SetVectorParameterValue(this, ParamterCollection, FName("EndPoint"), FLinearColor(PredictResult.PathData.Last().Location));
		UKismetMaterialLibrary::SetScalarParameterValue(this, ParamterCollection, FName("EndFalloff"), 0.0f);
	}
}
