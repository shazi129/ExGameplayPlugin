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
	virtual void Initialize(UWidget* Widget) override;
	virtual void NativeExecute() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName SwitcherName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int WidgetIndex;
		
private:
	UWidgetSwitcher* Switcher;
};