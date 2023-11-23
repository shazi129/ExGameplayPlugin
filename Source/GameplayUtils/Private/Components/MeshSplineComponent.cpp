#include "Components/MeshSplineComponent.h"
#include "GameplayUtilsModule.h"

void UMeshSplineComponent::BeginPlay()
{
	Super::BeginPlay();
	InitResetData();
}

void UMeshSplineComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}


void UMeshSplineComponent::RebuildSpline(const TArray<FVector>& Points)
{
	if (Points.Num() < 2)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, 2 points at least"), *FString(__FUNCTION__));
		return;
	}

	ClearSplinePoints(true);

	for (int i = 0; i < Points.Num(); i++)
	{
		AddSplinePoint(Points[i], ESplineCoordinateSpace::World, false);
		SetSplinePointType(i, ESplinePointType::Linear, true);
	}
	UpdateSpline();

	if (SplineMeshClass != nullptr)
	{
		MeshesInDisplay.Reset();
		GetSplineMeshes(Points.Num() - 1, MeshesInDisplay);

		FVector StartLocation;
		FVector StartTangent;
		FVector EndLocation;
		FVector EndTangent;
		for (int i = 0; i < MeshesInDisplay.Num(); i++)
		{
			GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent, ESplineCoordinateSpace::Local);
			GetLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent, ESplineCoordinateSpace::Local);
			MeshesInDisplay[i]->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);
			MeshesInDisplay[i]->SetVisibility(ShowMesh);
		}
	}
}

FVector UMeshSplineComponent::GetVectorToLocation(FVector Location)
{
	float NearestT = FindInputKeyClosestToWorldLocation(Location);
	FVector NearestLocationOnSpline = GetLocationAtSplineInputKey(NearestT, ESplineCoordinateSpace::World);
	return Location - NearestLocationOnSpline;
}

void UMeshSplineComponent::InitResetData()
{
	AddSplinePoint(ResetLocation, ESplineCoordinateSpace::World, false);
	SetSplinePointType(0, ESplinePointType::Linear, true);

	AddSplinePoint(ResetLocation + FVector(1, 1, 1), ESplineCoordinateSpace::World, false);
	SetSplinePointType(1, ESplinePointType::Linear, true);

	GetLocationAndTangentAtSplinePoint(0, ResetStartLocation, ResetStartTangent, ESplineCoordinateSpace::Local);
	GetLocationAndTangentAtSplinePoint(1, ResetEndLocation, ResetEndTangent, ESplineCoordinateSpace::Local);

	ClearSplinePoints(true);
}

void UMeshSplineComponent::GetSplineMeshes(int Num, TArray<USplineMeshComponent*>& OutSplineMeshes)
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
			MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &UMeshSplineComponent::NativeOnSplineMeshBeginOverlap);
			MeshComponent->OnComponentEndOverlap.AddDynamic(this, &UMeshSplineComponent::NativeOnSplineMeshEndOverlap);
			SplineMeshes.Add(MeshComponent);
		}
	}

	for (int i = 0; i < Num; i++)
	{
		SplineMeshes[i]->SetVisibility(ShowMesh);
		SplineMeshes[i]->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		OutSplineMeshes.Add(SplineMeshes[i]);
	}
	for (int i = Num; i < SplineMeshes.Num(); i++)
	{
		SplineMeshes[i]->SetVisibility(false);
		SplineMeshes[i]->SetStartAndEnd(ResetStartLocation, ResetStartTangent, ResetEndLocation, ResetEndTangent, true);
		SplineMeshes[i]->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void UMeshSplineComponent::ResetSplineMeshes()
{
	for (USplineMeshComponent* Component : SplineMeshes)
	{
		Component->SetVisibility(false);
		Component->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		Component->SetStartAndEnd(ResetStartLocation, ResetStartTangent, ResetEndLocation, ResetEndTangent, true);
	}
}

void UMeshSplineComponent::NativeOnSplineMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnSplineMeshBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void UMeshSplineComponent::NativeOnSplineMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OnSplineMeshEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
