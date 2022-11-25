#pragma once

#include "CoreMinimal.h"
#include "WidgetInteractAction.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class EXUMG_API UWidgetInteractAction : public UObject
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(UWidget* Widget)
	{
		Execute(Widget);
	}

	UFUNCTION(BlueprintImplementableEvent)
		void Execute(const UWidget* Widget);

};