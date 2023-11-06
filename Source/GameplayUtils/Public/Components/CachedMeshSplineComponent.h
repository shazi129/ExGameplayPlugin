#pragma once

#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SplineMeshComponent.h"
#include "CachedMeshSplineComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UCachedMeshSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Display(const TArray<FVector>& Points);

	UFUNCTION(BlueprintCallable)
	void Hide();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Class")
	TSubclassOf<USplineMeshComponent> SplineMeshClass;

protected:
	virtual void GetSplineMeshes(int Num, TArray<USplineMeshComponent*>& OutSplineMeshes);
	virtual void ResetSplineMeshes();

protected:
	// 轨迹mesh
	TArray<USplineMeshComponent*> SplineMeshes;
};