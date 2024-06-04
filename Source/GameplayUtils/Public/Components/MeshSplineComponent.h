#pragma once

/**
 * 一个带mesh的样条组件，当然也可以不配mesh，用作样条的普通扩展 
 */

#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SplineMeshComponent.h"
#include "MeshSplineComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UMeshSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UMeshSplineComponent();
	virtual void BeginPlay();
	virtual void EndPlay(EEndPlayReason::Type Reason);

	//
	UFUNCTION(BlueprintCallable)
	void RebuildSpline(const TArray<FVector>& Points);

	UFUNCTION(BlueprintCallable)
	void RebuildLine(const FVector& StartPoint, const FVector& EndPoint);

	//spline上到location最近的一个点到location的向量
	UFUNCTION(BlueprintCallable)
	FVector GetVectorToLocation(FVector Location);

	//重置当前的mesh
	UFUNCTION(BlueprintCallable)
	void ResetSplineMeshes();

public:
	//mesh类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cached Mesh")
	TSubclassOf<USplineMeshComponent> SplineMeshClass;

	//轨迹持续的时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cached Mesh")
	bool ShowMesh = true;

	//当mesh被回收时，会放到这个位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cached Mesh")
	FVector ResetLocation;

	//Spline的碰撞配置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cached Mesh")
	TEnumAsByte<ECollisionEnabled::Type> CollisionType;

	//Spline的起始缩放
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cached Mesh")
	FVector2D StartScale;

	//Spline的末端缩放
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cached Mesh")
	FVector2D EndScale;

protected:

	virtual void InitResetData();

	//获取spline mesh
	virtual void GetSplineMeshes(int Num, TArray<USplineMeshComponent*>& OutSplineMeshes);

	UFUNCTION()
	virtual void NativeOnSplineMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void NativeOnSplineMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSplineMeshOverlap"))
	void OnSplineMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSplineMeshOverlap"))
	void OnSplineMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// 轨迹mesh
	TArray<USplineMeshComponent*> SplineMeshes;

	//正在展示的Mesh
	TArray<USplineMeshComponent*> MeshesInDisplay;

	//reset状态下mesh的位置
	FVector ResetStartLocation;
	FVector ResetStartTangent;
	FVector ResetEndLocation;
	FVector ResetEndTangent;

};