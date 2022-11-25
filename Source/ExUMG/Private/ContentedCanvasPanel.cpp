#include "ContentedCanvasPanel.h"

UWidget* UContentedCanvasPanel::GetContent(FName Name)
{
	for (int i = 0; i < Contents.Num(); i++)
	{
		if (Contents[i].WidgetName == Name)
		{
			return Contents[i].Widget;
		}
	}
	return nullptr;
}

UWidget* UContentedCanvasPanel::GetChildByClass(TSubclassOf<UWidget> ChildClass)
{
	for (UPanelSlot* ChildSlot : Slots)
	{
		if (ChildSlot->Content && ChildSlot->Content.IsA(ChildClass))
		{
			return ChildSlot->Content;
		}
	}
	return nullptr;
}

TArray<UWidget*> UContentedCanvasPanel::GetAllChildByClass(TSubclassOf<UWidget> ChildClass)
{
	TArray<UWidget*> Result;
	for (UPanelSlot* ChildSlot : Slots)
	{
		if (ChildSlot->Content && ChildSlot->Content.IsA(ChildClass))
		{
			Result.Add(ChildSlot->Content);
		}
	}
	return Result;
}