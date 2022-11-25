#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputReceiverComponent.h"
#include "GameplayTags.h"
#include "ExInputSubsystem.generated.h"

UCLASS(BlueprintType)
class EXINPUTSYSTEM_API UExInputSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UExInputSubsystem* GetInputSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
		static FInputHandleEvent CreateInputEvent(const FGameplayTag& GameplayTag, const FInputHandleDelegate& Delegate);

	UFUNCTION(BlueprintCallable)
		void AddInputEvent(const FInputHandleEvent& InputHandleEvent);

	UFUNCTION(BlueprintCallable)
		void RemoveInputEvent(const FInputHandleEvent& InputHandleEvent);

	FInputHandleResult HandleInputEvent(const FGameplayTag& InputTag);

private:
	UPROPERTY()
		TArray<FInputHandleEvent> InputHandleEvents;
};