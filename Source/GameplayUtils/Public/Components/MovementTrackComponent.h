#pragma once

/**
 *  用于显示一个运动物体的轨迹
 *  
 *  用法：
 *  1. 创建一个Actor，在上面挂上UMovementTrackComponent
 *  2. 需要显示某个物体（例如Pawn）轨迹时：MovementTrackComponent->StartTrack(Pawn)
 *  3. 结束显示轨迹，MovementTrackComponent->StopTrack()
 * 
 *  在StartTrack后，MovementTrackComponent会一直持有Pawn，直至轨迹消失，表示轨迹是由谁生成，可以调用如下函数操作它
 *  MovementTrackComponent->GetTrackOwner() //获取轨迹所有者
 *  MovementTrackComponent->ClearTrackOwner() //清除轨迹所有者
 * 
 *  注意：StopTrack()并不会清除轨迹所有者
 */

#include "Components/MeshSplineComponent.h"
#include "MovementTrackComponent.generated.h"

//每段轨迹的信息
USTRUCT()
struct FMomentTrackSegment
{
	GENERATED_BODY()

	FVector StartLocation; //轨迹开始点
	int64 ExpireTime = 0; //轨迹消失时间
};

USTRUCT()
struct FSplineMeshOverlapInfo
{
	GENERATED_BODY();

	UPrimitiveComponent* OverlappedComp;
	AActor* OtherActor;
	UPrimitiveComponent* OtherComp;
	int32 OtherBodyIndex;
	bool bFromSweep;
	FHitResult SweepResult;

	FSplineMeshOverlapInfo(){};
	FSplineMeshOverlapInfo(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	int bDuringEnd = false;
	int EndNotifyDelay = 1;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UMovementTrackComponent : public UMeshSplineComponent
{
	GENERATED_BODY()

public:
	UMovementTrackComponent();

	virtual void BeginPlay();
	virtual void EndPlay(EEndPlayReason::Type Reason);
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	UFUNCTION(BlueprintCallable)
	void StartTrack(AActor* InTrackOwner);

	UFUNCTION(BlueprintCallable)
	void StopTrack();

	UFUNCTION(BlueprintCallable)
	AActor* GetTrackOwner();

	UFUNCTION(BlueprintCallable)
	void ClearTrackOwner();

	virtual void NativeOnSplineMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void NativeOnSplineMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

protected:
	void UpdateTrack();
	FMomentTrackSegment& AddSegment(const FVector& InStartLocation, const int64& AddTimestamp);

public:

	//静止时轨迹大小, 相对于TrackOwner朝向的offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementTrack")
	float StillnessOffset = 100;

	//每段轨迹的长度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementTrack")
	float SampleDistance = 50;

	//轨迹持续的时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementTrack")
	float SegmentDuration = 2;

	//overlap响应延迟
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementTrack")
	float OverlapDelay = 3;

	//overlap响应延迟
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementTrack")
	TArray<TSubclassOf<AActor>> OverlapActorClasses;

protected:
	bool bStartTrack = false;

	//轨迹点记录
	TArray<FMomentTrackSegment> TrackSegments;

	//Owner
	TWeakObjectPtr<AActor> TrackOwner;
	TArray<FSplineMeshOverlapInfo> OverlapRecordList;
};