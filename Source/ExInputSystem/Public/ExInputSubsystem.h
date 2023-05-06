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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputHandleMulticastDelegate, const FGameplayTag&, InputTag);

USTRUCT(BlueprintType)
struct EXINPUTSYSTEM_API FInputHandleEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		int32 EventID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag InputTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FInputHandleDelegate InputHandleDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool ExecuteOnlyOnce = false;
};

UCLASS(BlueprintType)
class EXINPUTSYSTEM_API UExInputSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UExInputSubsystem* GetInputSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static void TriggerInputTag(const UObject* WorldContextObject, const FGameplayTag& InputTag);

	FInputHandleEvent* AddInputEvent(const FGameplayTag& InputTag, int32 Priority = 0);
	bool RemoveInputEvent(int32 EventID);

	FInputHandleResult HandleInputEvent(const FGameplayTag& InputTag);

	int32 FindInputEventIndex(int32 EventID);

public:
	UPROPERTY(BlueprintAssignable)
		FInputHandleMulticastDelegate OnInputEventReceiveDelegate;

private:
	UPROPERTY()
		TArray<FInputHandleEvent> InputHandleEvents;

	std::atomic<int32> EventIDGenerator = 1;
};