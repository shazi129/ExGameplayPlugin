#include "Components/WidgetInteractActions/SwitcherTabAction.h"
#include "UMGLibrary.h"

void  USwitcherTabAction::Initialize(UWidget* Widget)
{
	Super::Initialize(Widget);

	Switcher = nullptr;

	if (!SwitcherName.IsNone())
	{
		if (UUserWidget* OwningWidget = UUMGLibrary::GetOwningUserWidget(Widget))
		{
			if (FObjectProperty* Property = FindFieldChecked<FObjectProperty>(OwningWidget->GetClass(), SwitcherName))
			{
				UObject* Object = Property->GetPropertyValue_InContainer(OwningWidget);
				Switcher = Cast<UWidgetSwitcher>(Object);
			}
		}
	}
}

void USwitcherTabAction::NativeExecute()
{
	if (Switcher)
	{
		Switcher->SetActiveWidgetIndex(WidgetIndex);
	}
}
