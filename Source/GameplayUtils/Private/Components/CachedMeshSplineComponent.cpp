#include "Components/CachedMeshSplineComponent.h"
#include "GameplayUtilsModule.h"

void UCachedMeshSplineComponent::Display(const TArray<FVector>& Points)
{
	if (Points.Num() < 2)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, 2 points at least"), *FString(__FUNCTION__));
		return;
	}

	ClearSplinePoints(true);
	ResetSplineMeshes();

	if (Points.Num() > 0)
	{
		for (int i = 0; i < Points.Num(); i++)
		{
			AddSplinePoint(Points[i], ESplineCoordinateSpace::World, false);
			SetSplinePointType(i, ESplinePointType::Linear, true);
		}
		UpdateSpline();

		TArray<USplineMeshComponent*> MeshComponents;
		GetSplineMeshes(Points.Num() - 1, MeshComponents);

		FVector StartLocation;
		FVector StartTangent;
		FVector EndLocation;
		FVector EndTangent;
		for (int i = 0; i < MeshComponents.Num(); i++)
		{
			GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent, ESplineCoordinateSpace::Local);
			GetLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent, ESplineCoordinateSpace::Local);
			MeshComponents[i]->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);
		}
	}
}

void UCachedMeshSplineComponent::Hide()
{
	ClearSplinePoints(true);
	ResetSplineMeshes();
}

void UCachedMeshSplineComponent::GetSplineMeshes(int Num, TArray<USplineMeshComponent*>& OutSplineMeshes)
{
	OutSplineMeshes.Reset();
	int NewMeshNum = Num - SplineMeshes.Num();
	if (NewMeshNum > 0)
	{
		if (SplineMeshClass == nullptr)
		{
			GAMEPLAYUTILS_LOG(Error, TEXT("%s error, SplineMeshClass is null"), *FString(__FUNCTION__));
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

void UCachedMeshSplineComponent::ResetSplineMeshes()
{
	for (USplineMeshComponent* Component : SplineMeshes)
	{
		Component->SetVisibility(false);
	}
}