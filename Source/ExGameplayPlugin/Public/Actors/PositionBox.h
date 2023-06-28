#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameplayTags.h"
#include "PositionBox.generated.h"

UCLASS()
class EXGAMEPLAYPLUGIN_API APositionBox : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = RandomPosition, meta = (WorldContextObject = "WorldContextObject"))
		static APositionBox* GetPositionBox(const UObject* WorldContextObject, const FGameplayTag& PositionTag);

	UFUNCTION(BlueprintPure, Category = RandomPosition, meta = (WorldContextObject = "WorldContextObject"))
		static bool GetBoxRandomPosition(const UObject* WorldContextObject, const FGameplayTag& PositionTag, FVector& Position);

	UFUNCTION(BlueprintPure, Category = RandomPosition, meta = (WorldContextObject = "WorldContextObject"))
		static bool GetBoxBound(const UObject* WorldContextObject, const FGameplayTag& PositionTag, FVector& Min, FVector& Max);

public:
	APositionBox(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure)
		FVector GetRandomPosition();

	UFUNCTION(BlueprintPure)
		void GetBound(FVector& Min, FVector& Max);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FGameplayTag PositionTag;
};

