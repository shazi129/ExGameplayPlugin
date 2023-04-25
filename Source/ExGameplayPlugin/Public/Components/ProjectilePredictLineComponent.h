#pragma once

#include "Components/SplineComponent.h"
#include "ProjectilePredictLineComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UProjectilePredictLineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void SetEmitParam(FVector InStartPosition, FVector InStartVelocity);

	UFUNCTION(BlueprintCallable)
		void SetIgnoreActors(TArray<AActor*> InActorsToIgnore);

	UFUNCTION(BlueprintCallable)
	void UpdatePredictLine();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
	bool TracePath = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Predict Path")
		float GrenadeRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Predict Path")
		TEnumAsByte<ECollisionChannel> TraceChannel;

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

protected:
	FVector StartPosition;
	FVector StartVelocity;

	TArray<TObjectPtr<AActor>> ActorsToIgnore;
};