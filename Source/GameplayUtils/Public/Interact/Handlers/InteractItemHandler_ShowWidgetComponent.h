#pragma once

/**
 * 根据交互数据展示或隐藏WidgetComponent
 */

#include "Components/WidgetComponent.h"
#include "Interact/Handlers/InteractItemHandler.h"
#include "InteractItemHandler_ShowWidgetComponent.generated.h"


UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class GAMEPLAYUTILS_API UInteractItemHandler_ShowWidgetComponent : public UInteractItemHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInteractInstanceData& InstanceData);

	UPROPERTY(EditAnywhere)
	FName WidgetComponentName;

protected:
	virtual void NativeOnSourceObjectChange(UObject* OldSourceObject, UObject* NewSourceObject) override;

	TWeakObjectPtr<UWidgetComponent> WidgetComponent;
};
