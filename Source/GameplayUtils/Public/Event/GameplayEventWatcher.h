#pragma once

#include "GameplayObject.h"
#include "InstancedStruct.h"
#include "GameplayEventWatcher.generated.h"

class UGameplayEventWatcher;

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FGameplayEventContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	UObject* Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EventTag;

	UPROPERTY(BlueprintReadWrite)
	FInstancedStruct Data;

	FGameplayEventContext(){}
	FGameplayEventContext(UObject* InInstigator);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayEventWatcherDelegate, const FGameplayEventContext&,  Context);

UCLASS(Abstract, Blueprintable, EditInlineNew)
class GAMEPLAYUTILS_API UGameplayEventWatcher : public UGameplayObject
{
	GENERATED_BODY()

public:
	virtual void Activate();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Activate"))
	void ReceiveActivate();

	virtual void Deactivate();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Deactivate"))
	void ReceiveDeactivate();

public:
	UPROPERTY(BlueprintAssignable)
	FGameplayEventWatcherDelegate SuccessDelegate;
};