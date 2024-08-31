#pragma once

#include "GameplayObject.h"
#include "InstancedStruct.h"
#include "GameplayTags.h"
#include "GameplayEventHandler.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class GAMEPLAYUTILS_API UGameplayEventHandler : public UGameplayObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void NativeExecute(const FGameplayEventContext& Context)
	{
		Execute(Context);
	}

	UFUNCTION(BlueprintImplementableEvent)
	void Execute(const FGameplayEventContext& Context);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer HandlerTags;
};