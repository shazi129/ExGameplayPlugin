#pragma once

/**
 * 对UObject的扩展
 * 1. 重写了GetWorld, 使蓝图子类能使用一些函数
 * 2. 增加了SourceObject，使其可溯源
 */

#include "CoreMinimal.h"
#include "GameplayObject.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class GAMEPLAYUTILS_API UGameplayObject : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

public:
	UFUNCTION(BlueprintPure)
	inline UObject* GetSourceObject() { return SourceObject.Get(); }

	UFUNCTION(BlueprintPure)
	AActor* GetSourceActor();

	UFUNCTION(BlueprintCallable)
	virtual void SetSourceObject(UObject* Object);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSourceObjectChange(UObject* OldSourceObject, UObject* NewSourceObject);

protected:
	virtual void NativeOnSourceObjectChange(UObject* OldSourceObject, UObject* NewSourceObject);

protected:
	UPROPERTY()
	TWeakObjectPtr<UObject> SourceObject;
};