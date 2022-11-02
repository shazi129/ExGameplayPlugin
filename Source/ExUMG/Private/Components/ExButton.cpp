#include "Components/ExButton.h"

TSharedRef<SWidget> UExButton::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	OnRebuildWidget();
	return Widget;
}