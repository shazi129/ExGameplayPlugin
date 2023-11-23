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

	for (int i = OverlapRecordList.Num() - 1; i >= 0; i--)
	{
		FSplineMeshOverlapInfo& OverlapInfo = OverlapRecordList[i];
		if (OverlapInfo.bDuringEnd == false)
		{
			continue;
		}
		OverlapInfo.EndNotifyDelay--;
		if (OverlapInfo.EndNotifyDelay <= 0)
		{
			if (OverlapRecordList.Num() == 1)
			{
				OnSplineMeshEndOverlap(OverlapInfo.OverlappedComp, OverlapInfo.OtherActor, OverlapInfo.OtherComp, OverlapInfo.OtherBodyIndex);
			}
			OverlapRecordList.RemoveAt(i);
		}
	}
}

void UMovementTrackComponent::StartTrack(AActor* InTrackOwner)
{
	bStartTrack = true;
	TrackOwner = InTrackOwner;
}

void UMovementTrackComponent::StopTrack()
{
	bStartTrack = false;
}

AActor* UMovementTrackComponent::GetTrackOwner()
{
	return TrackOwner.Get();
}

void UMovementTrackComponent::ClearTrackOwner()
{
	TrackOwner = nullptr;
}

void UMovementTrackComponent::NativeOnSplineMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//overlap对本家不生效
	if (OtherActor == TrackOwner)
	{
		return;
	}
	FSplineMeshOverlapInfo* NewOverlapInfo = new(OverlapRecordList) FSplineMeshOverlapInfo(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	NewOverlapInfo->EndNotifyDelay = OverlapDelay;
	NewOverlapInfo->bDuringEnd = false;
	if (OverlapRecordList.Num() == 1)
	{
		OnSplineMeshBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}

void UMovementTrackComponent::NativeOnSplineMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//overlap对本家不生效
	if (OtherActor == TrackOwner)
	{
		return;
	}

	for (FSplineMeshOverlapInfo& OverlapInfo : OverlapRecordList)
	{
		if (OverlapInfo.OverlappedComp == OverlappedComponent && OverlapInfo.OtherActor == OtherActor && OverlapInfo.OtherComp == OtherComp)
		{
			OverlapInfo.bDuringEnd = true;
		}
	}
}

FMomentTrackSegment& UMovementTrackComponent::AddSegment(const FVector& InStartLocation, const int64& AddTimestamp)
{
	TrackSegments.Emplace();
	FMomentTrackSegment& Segment = TrackSegments.Last();
	Segment.StartLocation = InStartLocation;

	if (SegmentDuration > 0)
	{
		Segment.ExpireTime = AddTimestamp + (int64)(SegmentDuration * 1000);
	}
	else
	{
		Segment.ExpireTime = 0;
	}
	return Segment;
}

void UMovementTrackComponent::UpdateTrack()
{
	int64 CurrentTime = UTimeHelperLibrary::GetTimestampMs();

	//不管什么状态，都要删掉过期的Segment
	int SegementIndex = TrackSegments.Num() - 1;
	while (SegementIndex >= 0)
	{
		FMomentTrackSegment& Segment = TrackSegments[SegementIndex];
		if (Segment.ExpireTime > 0 && CurrentTime > Segment.ExpireTime)
		{
			TrackSegments.RemoveAt(SegementIndex);
		}
		SegementIndex--;
	}

	FVector Offset(1, 1, 1);

	//只有开启的时候才往上加
	if (bStartTrack && TrackOwner.IsValid())
	{
		FVector CurrentLocation = TrackOwner->GetActorLocation();

		//当前是空的, 至少要有两个点
		if (TrackSegments.Num() < 2)
		{
			TrackSegments.Reset();
			AddSegment(CurrentLocation - TrackOwner->GetActorForwardVector() * StillnessOffset, CurrentTime);
			AddSegment(CurrentLocation, CurrentTime);
		}
		else
		{
			//当前位置与上次采样点的距离
			FVector& LastStart = TrackSegments[TrackSegments.Num() - 2].StartLocation;
			FVector& LastEnd = TrackSegments[TrackSegments.Num() - 1].StartLocation;
			if (!(CurrentLocation - LastEnd).IsNearlyZero())
			{
				float Distance = (CurrentLocation - LastStart).Size();
				if (Distance >= SampleDistance)
				{
					AddSegment(CurrentLocation, CurrentTime);
				}
				else
				{
					TrackSegments.Last().StartLocation = CurrentLocation;
				}
			}
		}
	}

	//已经不需要展示轨迹了
	if (TrackSegments.Num() < 2)
	{
		ResetSplineMeshes();
		TrackSegments.Reset();
		return;
	}


	//根据各点展示SplineMesh
	TArray<FVector> TrackPoints;
	for (const auto& Segment : TrackSegments)
	{
		TrackPoints.Add(Segment.StartLocation);
	}
	RebuildSpline(TrackPoints);
}

FSplineMeshOverlapInfo::FSplineMeshOverlapInfo(UPrimitiveComponent* InOverlappedComp, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult)
	: OverlappedComp(InOverlappedComp)
	, OtherActor(InOtherActor)
	, OtherComp(InOtherComp)
	, OtherBodyIndex(InOtherBodyIndex)
	, bFromSweep(bInFromSweep)
	, SweepResult(InSweepResult)
{
}
