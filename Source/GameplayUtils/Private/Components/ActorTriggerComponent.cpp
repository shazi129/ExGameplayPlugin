#include "Components/ActorTriggerComponent.h"
#include "Components/ShapeComponent.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"

void UActorTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto TiggerShapeName : TiggerShapeNames)
	{
		auto Component = UGameplayUtilsLibrary::GetComponentByName(GetOwner(), TiggerShapeName, UPrimitiveComponent::StaticClass());
		if (auto ShapeComponent = Cast<UPrimitiveComponent>(Component))
		{
			ShapeComponent->OnComponentBeginOverlap.AddDynamic(this, &UActorTriggerComponent::NativeBeginOverlap);
			ShapeComponent->OnComponentEndOverlap.AddDynamic(this, &UActorTriggerComponent::NativeEndOverlap);
		}
	}
}

void UActorTriggerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UActorTriggerComponent::NativeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsTargetActor(OtherActor, OtherComp))
	{
		return;
	}

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
		FTriggeredActorInfo* OverlapInfo = new (OverlappingInfoList)FTriggeredActorInfo();
		OverlapInfo->Actor = OtherActor;
		OverlapInfo->Components.Add(OtherComp);
		OnTriggerBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	}
}

void UActorTriggerComponent::NativeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsTargetActor(OtherActor, OtherComp))
	{
		return;
	}

	int RemoveIndex = -1;
	for (int ActorIndex = OverlappingInfoList.Num() - 1; ActorIndex >= 0; ActorIndex--)
	{
		FTriggeredActorInfo& OverlappingInfo = OverlappingInfoList[ActorIndex];
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

bool UActorTriggerComponent::IsTargetActor(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	bool Result = ActorClasses.IsEmpty();
	for (auto& ActorClassPtr : ActorClasses)
	{
		auto ActorClass = ActorClassPtr.LoadSynchronous();
		if (OtherActor->IsA(ActorClass))
		{
			Result = true;
			break;
		}
	}
	if (!Result)
	{
		return false;
	}

	Result = ComponentClasses.IsEmpty();
   	for (auto& ComponentClassPtr : ComponentClasses)
	{
		auto ComponentClass = ComponentClassPtr.LoadSynchronous();
		if (OtherComp->IsA(ComponentClass))
		{
			Result = true;
			break;
		}
	}

	return Result;
}


