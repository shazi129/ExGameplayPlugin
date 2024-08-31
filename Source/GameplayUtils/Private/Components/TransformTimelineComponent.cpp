#include "Components/TransformTimelineComponent.h"
#include "GameplayUtilsModule.h"

UTransformTimelineComponent::UTransformTimelineComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UTransformTimelineComponent::BeginPlay()
{
	Super::BeginPlay();

	FTransform Transform;

	//创建Location的Timeline
	LocationTrackName = "LocationCurve";
	LocationPropertyName = "DistanceInterp";
	LocationFinishEvent.BindDynamic(this, &UTransformTimelineComponent::OnLocationTimelineFinish);
	LocationUpdateEvent.BindDynamic(this, &UTransformTimelineComponent::OnLocationTimelineUpdate);
	LocationTimeline = Cast<UTimelineComponent>(GetOwner()->AddComponentByClass(UTimelineComponent::StaticClass(), true, Transform, true));
	LocationTimeline->SetTimelineFinishedFunc(LocationFinishEvent);
	LocationTimeline->SetLooping(false);
	LocationTimeline->SetIgnoreTimeDilation(true);
	LocationTimeline->AddInterpFloat(nullptr, LocationUpdateEvent, LocationPropertyName, LocationTrackName);
	LocationTimeline->RegisterComponent();
	LocationTimeline->RegisterAllComponentTickFunctions(true);

	//创建Rotation的Timeline
	RotationTrackName = "RotationCurve";
	RotationPropertyName = "RotationInterp";
	RotationFinishEvent.BindDynamic(this, &UTransformTimelineComponent::OnRotationTimelineFinish);
	RotationUpdateEvent.BindDynamic(this, &UTransformTimelineComponent::OnRotationTimelineUpdate);
	RotationTimeline = Cast<UTimelineComponent>(GetOwner()->AddComponentByClass(UTimelineComponent::StaticClass(), true, Transform, true));
	RotationTimeline->SetTimelineFinishedFunc(RotationFinishEvent);
	RotationTimeline->SetLooping(false);
	RotationTimeline->SetIgnoreTimeDilation(true);
	RotationTimeline->AddInterpFloat(nullptr, RotationUpdateEvent, RotationPropertyName, RotationTrackName);
	RotationTimeline->RegisterComponent();
	RotationTimeline->RegisterAllComponentTickFunctions(true);

	//创建Scale的Timeline
	ScaleTrackName = "ScaleCurve";
	ScalePropertyName = "ScaleInterp";
	ScaleFinishEvent.BindDynamic(this, &UTransformTimelineComponent::OnScaleTimelineFinish);
	ScaleUpdateEvent.BindDynamic(this, &UTransformTimelineComponent::OnScaleTimelineUpdate);
	ScaleTimeline = Cast<UTimelineComponent>(GetOwner()->AddComponentByClass(UTimelineComponent::StaticClass(), true, Transform, true));
	ScaleTimeline->SetTimelineFinishedFunc(ScaleFinishEvent);
	ScaleTimeline->SetLooping(false);
	ScaleTimeline->SetIgnoreTimeDilation(true);
	ScaleTimeline->AddInterpFloat(nullptr, ScaleUpdateEvent, ScalePropertyName, ScaleTrackName);
	ScaleTimeline->RegisterComponent();
	ScaleTimeline->RegisterAllComponentTickFunctions(true);
}

void UTransformTimelineComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UTransformTimelineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GAMEPLAYUTILS_LOG(Log, TEXT("UTransformTimelineComponent::TickComponent"));
}

void UTransformTimelineComponent::StartInterpolation(FTransform TargetTransform, UCurveFloat* LocationCurve, UCurveFloat* RotationCurve, UCurveFloat* ScaleCurve)
{
	StartInterpLocation(TargetTransform.GetLocation(), LocationCurve);
	StartInterpRotation(TargetTransform.GetRotation().Rotator(), RotationCurve);
	StartInterpScale(TargetTransform.GetScale3D(), ScaleCurve);
}

void UTransformTimelineComponent::StartInterpLocation(FVector TargetLocation, UCurveFloat* LocationCurve)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("%s, %s, %s"), *FString(__FUNCTION__), *TargetLocation.ToString(), *GetNameSafe(LocationCurve));

	if (!LocationCurve)
	{
		GetTargetActor()->SetActorLocation(TargetLocation);
		return;
	}

	StartLocation = GetTargetActor()->GetActorLocation();
	EndLocation = TargetLocation;
	LocationTimeline->Stop();
	LocationTimeline->SetFloatCurve(LocationCurve, LocationTrackName);
	LocationTimeline->PlayFromStart();
}

void UTransformTimelineComponent::OnLocationTimelineFinish()
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UTransformTimelineComponent::OnLocationTimelineFinish"));
	if (OnLocationInterpFinishDelegate.IsBound())
	{
		OnLocationInterpFinishDelegate.Broadcast();
	}
}

void UTransformTimelineComponent::OnLocationTimelineUpdate(float Value)
{
	FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Value);
	AActor* Actor = GetTargetActor();

	GAMEPLAYUTILS_LOG(Log, TEXT(" %s: target: %s, alpha: %f, location: %s"),*FString(__FUNCTION__), *GetNameSafe(Actor), Value, *NewLocation.ToString());
	if (OnLocationInterpUpdateDelegate.IsBound())
	{
		OnLocationInterpUpdateDelegate.Broadcast(Value);
	}

	if (Actor)
	{
		Actor->SetActorLocation(NewLocation);
	}
	
}

void UTransformTimelineComponent::StopInterpLocation()
{
	if (LocationTimeline)
	{
		LocationTimeline->Stop();
	}
}

void UTransformTimelineComponent::StartInterpRotation(FRotator TargetRotation, UCurveFloat* RotationCurve)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("%s, %s, %s"), *FString(__FUNCTION__), *TargetRotation.ToString(), *GetNameSafe(RotationCurve));
	if (!RotationCurve)
	{
		GetTargetActor()->SetActorRotation(TargetRotation);
		return;
	}

	StartRotation = GetTargetActor()->GetActorRotation();
	EndRotation = TargetRotation;
	RotationTimeline->Stop();
	RotationTimeline->SetFloatCurve(RotationCurve, RotationTrackName);
	RotationTimeline->PlayFromStart();
}

void UTransformTimelineComponent::StartInterpScale(FVector TargetScale, UCurveFloat* ScaleCurve)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("%s, %s, %s"), *FString(__FUNCTION__), *TargetScale.ToString(), *GetNameSafe(ScaleCurve));
	if (!ScaleCurve)
	{
		GetTargetActor()->SetActorScale3D(TargetScale);
		return;
	}

	StartScale = GetTargetActor()->GetActorScale3D();
	EndScale = TargetScale;
	ScaleTimeline->Stop();
	ScaleTimeline->SetFloatCurve(ScaleCurve, ScaleTrackName);
	ScaleTimeline->PlayFromStart();
}

void UTransformTimelineComponent::SetTargetActor(AActor* Actor)
{
	TargetActor = Actor;
}

AActor* UTransformTimelineComponent::GetTargetActor()
{
	return TargetActor.IsValid() ? TargetActor.Get() : GetOwner();
}

void UTransformTimelineComponent::OnRotationTimelineFinish()
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UTransformTimelineComponent::OnRotationTimelineFinish"));
	if (OnRotationInterpFinishDelegate.IsBound())
	{
		OnRotationInterpFinishDelegate.Broadcast();
	}
}

void UTransformTimelineComponent::OnRotationTimelineUpdate(float Value)
{
	//GAMEPLAYUTILS_LOG(Log, TEXT("UTransformTimelineComponent::OnRotationTimelineUpdate: %f"), Value);
	if (OnRotationInterpUpdateDelegate.IsBound())
	{
		OnRotationInterpUpdateDelegate.Broadcast(Value);
	}

	FQuat NewQuat = FMath::Lerp(StartRotation.Quaternion(), EndRotation.Quaternion(), Value);
	GetTargetActor()->SetActorRotation(NewQuat);
}

void UTransformTimelineComponent::OnScaleTimelineFinish()
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UTransformTimelineComponent::OnScaleTimelineFinish"));
	if (OnScaleInterpFinishDelegate.IsBound())
	{
		OnScaleInterpFinishDelegate.Broadcast();
	}
}

void UTransformTimelineComponent::OnScaleTimelineUpdate(float Output)
{
	//GAMEPLAYUTILS_LOG(Log, TEXT("UTransformTimelineComponent::OnScaleTimelineUpdate: %f"), Output);
	if (OnScaleInterpUpdateDelegate.IsBound())
	{
		OnScaleInterpUpdateDelegate.Broadcast(Output);
	}
}
