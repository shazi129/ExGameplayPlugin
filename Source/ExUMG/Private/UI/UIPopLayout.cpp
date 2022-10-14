#include "UI/UIPopLayout.h"
#include "UI/UIPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "ExUMGModule.h"

UUIPopLayout::UUIPopLayout(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PopName = "";
}

void UUIPopLayout::InitPopLayout(const FPopLayoutParam& PopLayoutParam)
{
	PopName = PopLayoutParam.PopName;
}

UUIPanel* UUIPopLayout::GetParentPanel()
{
	return ParentPanel;
}

void UUIPopLayout::SetParentPanel(UUIPanel* Panel)
{
	ParentPanel = Panel;
}

void UUIPopLayout::Dispose()
{
	this->RemoveFromParent();
}

UUserWidget* UUIPopLayout::SetContent(TSubclassOf<UUserWidget> ContentWidgetClass)
{
	if (ContentWidgetClass == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPopLayout SetContent Error, ContentWidgetClass is NULL"));
		return nullptr;
	}

	UPanelWidget* ContentPanel = Cast<UPanelWidget>(this->WidgetTree->FindWidget("ContentPanel"));
	if (ContentPanel == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPopLayout SetContent Error, Cannot find ContentPanel"));
		return nullptr;
	}

	ContentWidget = CreateWidget(GetWorld(), ContentWidgetClass);
	if (ContentWidget == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPopLayout SetContent Error, Create PopLayout Content Widget return NULL"));
		return nullptr;
	}

	ContentPanel->AddChild(ContentWidget);
	UUIPanel::FullFillWidget(ContentWidget);

	return ContentWidget;
}

UUserWidget* UUIPopLayout::GetContent()
{
	return ContentWidget;
}

UUIPopLayout* UUIPopLayout::GetPopLayoutByContent(UUserWidget* ContentWidget)
{
	if (ContentWidget == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPopLayout GetPopLayoutByContent Error, ContentWidget is NULL"));
		return nullptr;
	}

	UPanelWidget* PanelWidget = ContentWidget->GetParent();
	if (PanelWidget == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPopLayout GetPopLayoutByContent Error, ContentWidget has no Panrent"));
		return nullptr;
	}

	UObject* WidgetTreeObject = PanelWidget->GetOuter();
	if (WidgetTreeObject == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPopLayout GetPopLayoutByContent Error, Cannot get WidgetTree"));
		return nullptr;
	}

	UObject* PopLayoutObject = WidgetTreeObject->GetOuter();
	if (PopLayoutObject == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPopLayout GetPopLayoutByContent Error, Cannot Get PopLayoutObject"));
		return nullptr;
	}

	return  Cast<UUIPopLayout>(PopLayoutObject);
}