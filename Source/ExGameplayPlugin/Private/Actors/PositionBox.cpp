#include "Actors/PositionBox.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

APositionBox* APositionBox::GetPositionBox(const UObject* WorldContextObject, const FGameplayTag& PositionTag)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APositionBox::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		if (APositionBox* Box = Cast<APositionBox>(Actor))
		{
			if (Box->PositionTag.IsValid() && Box->PositionTag.MatchesTag(PositionTag))
			{
				return Box;
			}
		}
	}
	return nullptr;
}

bool APositionBox::GetBoxRandomPosition(const UObject* WorldContextObject, const FGameplayTag& PositionTag, FVector& Position)
{
	if (APositionBox* Box = APositionBox::GetPositionBox(WorldContextObject, PositionTag))
	{
		Position = Box->GetRandomPosition();
		return true;
	}
	return false;
}

bool APositionBox::GetBoxBound(const UObject* WorldContextObject, const FGameplayTag& PositionTag, FVector& Min, FVector& Max)
{
	if (APositionBox* Box = APositionBox::GetPositionBox(WorldContextObject, PositionTag))
	{
		Box->GetBound(Min, Max);
		return true;
	}
	return false;
}

APositionBox::APositionBox(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = BoxComponent;
}

FVector APositionBox::GetRandomPosition()
{
	FVector& Origin = BoxComponent->Bounds.Origin;
	FVector HalfSize = BoxComponent->Bounds.BoxExtent / 2.0f;
	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, HalfSize);
}

void APositionBox::GetBound(FVector& Min, FVector& Max)
{
	FVector& Origin = BoxComponent->Bounds.Origin;
	FVector HalfSize = BoxComponent->Bounds.BoxExtent / 2.0f;
	Min = Origin - HalfSize;
	Max = Origin + HalfSize;
}
