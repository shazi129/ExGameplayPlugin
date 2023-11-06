#pragma once

#include "Components/CachedMeshSplineComponent.h"
#include "MovementTrackComponent.generated.h"

USTRUCT()
struct FMomentTrackSegment
{
	GENERATED_BODY()

	FVector StartLocation;
	int64 ExpireTime = 0;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UMovementTrackComponent : public UCachedMeshSplineComponent
{
	GENERATED_BODY()

public:
	UMovementTrackComponent();

	virtual void BeginPlay();
	virtual void EndPlay(EEndPlayReason::Type Reason);
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	UFUNCTION(BlueprintCallable)
	void StartTrack();

	UFUNCTION(BlueprintCallable)
	void StopTrack();

protected:
	void UpdateTrack();
	FMomentTrackSegment& AddSegment(const FVector& InStartLocation, const int64& InExpireTime);

public:
	//每段轨迹的长度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementTrack")
	float SampleDistance = 50;

	//轨迹持续的时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementTrack")
	float SegmentDuration = 2;

protected:
	bool bStartTrack = false;

	//轨迹点记录
	TArray<FMomentTrackSegment> TrackSegments;
};