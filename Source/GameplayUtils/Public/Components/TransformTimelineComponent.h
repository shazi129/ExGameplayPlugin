#pragma once

/**
 * 通过timeline对Actor的Trasform进行插值的Component
 * 
 */

#include "CoreMinimal.h"
#include "GameplayTypes.h"
#include "Components/TimelineComponent.h"
#include "TransformTimelineComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UTransformTimelineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTransformTimelineComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	UFUNCTION(BlueprintCallable)
	void StartInterpolation(FTransform TargetTransform, UCurveFloat* LocationCurve, UCurveFloat* RotationCurve, UCurveFloat* ScaleCurve);

	//////////////////////////////////////位置相关插值
	UFUNCTION(BlueprintCallable)
	void StartInterpLocation(FVector TargetLocation, UCurveFloat* LocationCurve);

	UFUNCTION()
	void OnLocationTimelineFinish();

	UFUNCTION()
	void OnLocationTimelineUpdate(float Output);

	UFUNCTION(BlueprintCallable)
	void StopInterpLocation();

	//////////////////////////////////////旋转相关插值
	UFUNCTION(BlueprintCallable)
	void StartInterpRotation(FRotator TargetRotation, UCurveFloat* RotationCurve);

	UFUNCTION()
	void OnRotationTimelineFinish();

	UFUNCTION()
	void OnRotationTimelineUpdate(float Output);

	/////////////////////////////////////缩放相关插值
	UFUNCTION(BlueprintCallable)
	void StartInterpScale(FVector ScaleLocation, UCurveFloat* ScaleCurve);

	UFUNCTION()
	void OnScaleTimelineFinish();

	UFUNCTION()
	void OnScaleTimelineUpdate(float Output);

	UFUNCTION(BlueprintCallable)
	void SetTargetActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transform Timeline")
	FString TargetComponentName;

	//用于移动Timeline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Transform Timeline")
	TObjectPtr<UTimelineComponent> LocationTimeline;

	//用于旋转的Timeline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Transform Timeline")
	TObjectPtr<UTimelineComponent> RotationTimeline;

	//用于旋转的Timeline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform Timeline")
	TObjectPtr<UTimelineComponent> ScaleTimeline;

	//Location插值的相关回调
	UPROPERTY(BlueprintAssignable)
	FDynamicMulticastDelegate OnLocationInterpFinishDelegate;

	UPROPERTY(BlueprintAssignable)
	FFloatDynamicMulticastDelegate OnLocationInterpUpdateDelegate;

	//Rotation插值的相关回调
	UPROPERTY(BlueprintAssignable)
	FDynamicMulticastDelegate OnRotationInterpFinishDelegate;

	UPROPERTY(BlueprintAssignable)
	FFloatDynamicMulticastDelegate OnRotationInterpUpdateDelegate;

	//Scale插值的相关回调
	UPROPERTY(BlueprintAssignable)
	FDynamicMulticastDelegate OnScaleInterpFinishDelegate;

	UPROPERTY(BlueprintAssignable)
	FFloatDynamicMulticastDelegate OnScaleInterpUpdateDelegate;

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

private:
	//平移的一些信息
	FName LocationTrackName;
	FName LocationPropertyName;
	FVector StartLocation;
	FVector EndLocation;
	FOnTimelineEvent LocationFinishEvent;
	FOnTimelineFloat LocationUpdateEvent;

	//旋转的一些信息
	FName RotationTrackName;
	FName RotationPropertyName;
	FRotator StartRotation;
	FRotator EndRotation;
	FOnTimelineEvent RotationFinishEvent;
	FOnTimelineFloat RotationUpdateEvent;

	//缩放的一些信息
	FName ScaleTrackName;
	FName ScalePropertyName;
	FVector StartScale;
	FVector EndScale;
	FOnTimelineEvent ScaleFinishEvent;
	FOnTimelineFloat ScaleUpdateEvent;
};
