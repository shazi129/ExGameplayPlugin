#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTags.h"
#include "ExInputSubsystem.generated.h"

USTRUCT(BlueprintType)
struct EXINPUTSYSTEM_API FInputHandleResult
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		bool IsHandled = false;
};

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FInputHandleResult, FInputHandleDelegate, const FGameplayTag&, InputTag);

USTRUCT(BlueprintType)
struct EXINPUTSYSTEM_API FInputHandleEvent
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadOnly)
		int64 EventID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag InputTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FInputHandleDelegate InputHandleDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Priority = 0;
};

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