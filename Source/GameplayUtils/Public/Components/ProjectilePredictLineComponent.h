#pragma once

#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SplineMeshComponent.h"
#include "ProjectilePredictLineComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UProjectilePredictLineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UProjectilePredictLineComponent();

	virtual void BeginPlay();
	virtual void EndPlay(EEndPlayReason::Type Reason);
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	UFUNCTION(BlueprintCallable)
		void SetEmitParamByViewport();

	UFUNCTION(BlueprintPure)
		FVector GetStartVelocity();

	UFUNCTION(BlueprintCallable)
		void SetPredictLineVisible(bool Visible);

	UFUNCTION(BlueprintCallable)
		void SetIgnoreActors(TArray<AActor*> InActorsToIgnore);

	UFUNCTION(BlueprintCallable)
		void UpdatePredictLine();

	UFUNCTION(BlueprintCallable)
		void GetSplineMeshes(int Num, TArray<USplineMeshComponent*>& OutSplineMeshes);

	UFUNCTION(BlueprintCallable)
		void ResetSplineMeshes();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		FRotator EmitViewOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		float DefaultVelocityValue = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		bool TracePath = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Predict Path")
		float GrenadeRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		bool TraceComplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		float DrawDebugTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		float SimFrequency = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		float MaxSimulaTionTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		float OverrideGravityZ = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		UMaterialParameterCollection* ParamterCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		TSubclassOf<USplineMeshComponent> SplineMeshClass;;

protected:
	FVector StartPosition;
	FVector StartVelocity;

	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	TArray<USplineMeshComponent*> SplineMeshes;
};