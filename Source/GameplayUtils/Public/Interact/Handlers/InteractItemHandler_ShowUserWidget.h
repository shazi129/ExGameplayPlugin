#pragma once

/**
 * 根据交互数据展示或隐藏UserWidget
 */

#include "Components/WidgetComponent.h"
#include "Interact/Handlers/InteractItemHandler.h"
#include "InteractItemHandler_ShowUserWidget.generated.h"


UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class GAMEPLAYUTILS_API UInteractItemHandler_ShowUserWidget : public UInteractItemHandler
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString ButtonName;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UUserWidget> UserWidgetClass;

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UUserWidget> UserWidget;
};
