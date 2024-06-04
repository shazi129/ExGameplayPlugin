#include "Actors/MultiTriggerActor.h"
#include "Components/ShapeComponent.h"
#include "GameplayUtilsModule.h"

void AMultiTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> Components;
	GetComponents(UShapeComponent::StaticClass(), Components);
	for (auto Component : Components)
	{
		if (auto ShapeComponent = Cast<UShapeComponent>(Component))
		{
			ShapeComponent->OnComponentBeginOverlap.AddDynamic(this, &AMultiTriggerActor::NativeBeginOverlap);
			ShapeComponent->OnComponentEndOverlap.AddDynamic(this, &AMultiTriggerActor::NativeEndOverlap);
		}
	}
}

void AMultiTriggerActor::NativeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bool Found = false;
	for (auto& OverlappingInfo : OverlappingInfoList)
	{
		if (OtherActor == OverlappingInfo.Actor)
		{
			Found = true;
			OverlappingInfo.Components.AddUnique(OtherComp);
			break;
		}
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s, OtherActor:%s, OtherComp:%s, Found:%d"), *FString(__FUNCTION__), *GetNameSafe(OtherActor), *GetNameSafe(OtherComp), Found);

	if (!Found)
	{
		FActorOverlapInfo* OverlapInfo = new (OverlappingInfoList)FActorOverlapInfo();
		OverlapInfo->Actor = OtherActor;
		OverlapInfo->Components.Add(OtherComp);
		OnTriggerBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	}
}

void AMultiTriggerActor::NativeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	int RemoveIndex = -1;
	for (int ActorIndex = OverlappingInfoList.Num() - 1; ActorIndex >= 0; ActorIndex--)
	{
		FActorOverlapInfo& OverlappingInfo = OverlappingInfoList[ActorIndex];
		if (OtherActor == OverlappingInfo.Actor)
		{
			for (int CompIndex = OverlappingInfo.Components.Num() - 1; CompIndex >= 0; CompIndex--)
			{
				if (OverlappingInfo.Components[CompIndex] == OtherComp)
				{
					OverlappingInfo.Components.RemoveAt(CompIndex);
				}
			}

			if (OverlappingInfo.Components.Num() == 0)
			{
				RemoveIndex = ActorIndex;
				break;
			}
		}
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s, OtherActor:%s, OtherComp:%s, RemoveIndex:%d"), *FString(__FUNCTION__), *GetNameSafe(OtherActor), *GetNameSafe(OtherComp), RemoveIndex);
	if (OverlappingInfoList.IsValidIndex(RemoveIndex))
	{
		OverlappingInfoList.RemoveAt(RemoveIndex);
		OnTriggerEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	}
}
