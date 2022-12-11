#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "WidgetInteractAction.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew, abstract)
class EXUMG_API UWidgetInteractAction : public UObject
{
	GENERATED_BODY()

public:

	virtual void Initialize(UWidget* Widget) {};

	virtual void NativeExecute()
	{
		Execute();
	}

	UFUNCTION(BlueprintImplementableEvent)
		void Execute();
};