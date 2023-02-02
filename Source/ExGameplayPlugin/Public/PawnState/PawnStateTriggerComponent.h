#pragma once 

#include "CoreMinimal.h"
#include "PawnStateTypes.h"
#include "PawnStateTriggerComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UPawnStateTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditAnywhere, Category="PawnState")
	TArray<UPawnStateAsset*> PawnStateAssets;
};
