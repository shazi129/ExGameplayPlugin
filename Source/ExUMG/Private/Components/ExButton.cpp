#include "Components/ExButton.h"

TSharedRef<SWidget> UExButton::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	OnClicked.RemoveDynamic(this, &UExButton::HandleClickActions);
	OnClicked.AddDynamic(this, &UExButton::HandleClickActions);

	for (int i = 0; i < ClickActions.Num(); i++)
	{
		ClickActions[i]->Initialize(this);
	}

	return Widget;
}

void UExButton::HandleClickActions()
{
	for (int i = 0; i < ClickActions.Num(); i++)
	{
		if (ClickActions[i] != nullptr)
		{
			ClickActions[i]->NativeExecute();
		}
	}
}