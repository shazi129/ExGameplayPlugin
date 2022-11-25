#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WidgetInteractActions/WidgetInteractAction.h"
#include "SwitcherTabAction.generated.h"

UCLASS()
class EXUMG_API USwitcherTabAction : public UWidgetInteractAction
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(UWidget* Widget) override
	{
		if (Switcher)
		{
			Switcher->SetActiveWidgetIndex(WidgetIndex);
		}
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UWidgetSwitcher* Switcher;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int WidgetIndex;
};