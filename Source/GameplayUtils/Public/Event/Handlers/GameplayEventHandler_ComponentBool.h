#pragma once

#include "Event/GameplayEventHandler.h"
#include "GameplayEventHandler_ComponentBool.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FNameBoolData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	bool Value;
};

UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYUTILS_API UGameplayEventHandler_ComponentBool : public UGameplayEventHandler
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FGameplayEventContext MakeComponentBoolContext(AActor* Actor, const FString& ComponentName, bool ComponentValue);

public:
	virtual void NativeExecute(const FGameplayEventContext& Context);

	UFUNCTION(BlueprintNativeEvent)
	void HandleComponent(UActorComponent* Component, bool Value);
};