#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTags.h"
#include "ExInputTypes.h"
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
class EXINPUTSYSTEM_API UExInputSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UExInputSubsystem* GetInputSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static void TriggerInputTag(const UObject* WorldContextObject, const FGameplayTag& InputTag);

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	virtual void Deinitialize();

	UFUNCTION(BlueprintCallable)
		UInputActionHandler* CreateInputActionHandler(TSubclassOf<UInputActionHandler> InputHandlerClass);

	FInputHandleEvent* AddInputEvent(const FGameplayTag& InputTag, int32 Priority = 0);
	bool RemoveInputEvent(int32 EventID);

	FInputHandleResult HandleInputEvent(const FGameplayTag& InputTag);
	int32 FindInputEventIndex(int32 EventID);

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const;

public:
	UPROPERTY(BlueprintAssignable)
		FInputHandleMulticastDelegate OnInputEventReceiveDelegate;

private:
	UPROPERTY()
		TArray<FInputHandleEvent> InputHandleEvents;

	std::atomic<int32> EventIDGenerator = 1;

	UPROPERTY()
		TArray<UInputActionHandler*> HandlerInstance;
};