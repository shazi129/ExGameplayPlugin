#include "Components/MovementTrackComponent.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/TimeHelperLibrary.h"

UMovementTrackComponent::UMovementTrackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovementTrackComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMovementTrackComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}

void UMovementTrackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateTrack();
}

void UMovementTrackComponent::StartTrack()
{
	bStartTrack = true;
}

void UMovementTrackComponent::StopTrack()
{
	bStartTrack = false;
}

FMomentTrackSegment& UMovementTrackComponent::AddSegment(const FVector& InStartLocation, const int64& InExpireTime)
{
	TrackSegments.Emplace();
	FMomentTrackSegment& Segment = TrackSegments.Last();
	Segment.StartLocation = InStartLocation;
	Segment.ExpireTime = InExpireTime;
	return Segment;
}

void UMovementTrackComponent::UpdateTrack()
{
	if (!bStartTrack) return;

	int64 CurrentTime = UTimeHelperLibrary::GetTimestampMs();
	FVector CurrentLocation = GetComponentLocation();

	//删掉过期的Segment
	int SegementIndex = TrackSegments.Num() - 1;
	while (SegementIndex >= 0)
	{
		if (CurrentTime - TrackSegments[SegementIndex].ExpireTime >= SegmentDuration * 2000)
		{
			TrackSegments.RemoveAt(SegementIndex);
		}
		SegementIndex--;
	}

	if (TrackSegments.Num() == 0)
	{
		AddSegment(CurrentLocation, CurrentTime);
	}
	else
	{
		float Distance = (CurrentLocation - TrackSegments.Last().StartLocation).Size();
		if (Distance >= SampleDistance)
		{
			AddSegment(CurrentLocation, CurrentTime);
		}
	}

	TArray<FVector> TrackPoints;
	for (const auto& Segment : TrackSegments)
	{
		TrackPoints.Add(Segment.StartLocation);
	}
	TrackPoints.Add(CurrentLocation);

	Display(TrackPoints);

}



