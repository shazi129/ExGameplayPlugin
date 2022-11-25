#pragma once 

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "PawnStateComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UPawnStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool CanEnterState(const FGameplayTag& NewPawnState);

	UFUNCTION(BlueprintCallable)
	bool EnterState(const FGameplayTag& NewPawnState);

	UFUNCTION(BlueprintCallable)
	bool LeaveState(const FGameplayTag& PawnState);

private:
	FGameplayTagContainer CurrentPawnStates;
};