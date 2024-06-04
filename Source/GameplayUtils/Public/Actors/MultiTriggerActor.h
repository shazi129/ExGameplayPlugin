#pragma once

#include "CoreMinimal.h"
#include "MultiTriggerActor.generated.h"

USTRUCT()
struct GAMEPLAYUTILS_API FActorOverlapInfo
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> Actor;
	TArray<TWeakObjectPtr<UPrimitiveComponent>> Components;
};

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API AMultiTriggerActor : public AActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	virtual void NativeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void NativeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTriggerBeginOverlap"))
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTriggerEndOverlap"))
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	TArray<FActorOverlapInfo> OverlappingInfoList;
};