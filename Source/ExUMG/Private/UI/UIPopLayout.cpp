#include "GameUIPopLayout.h"
#include "GameUIPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "GameUIManagerModule.h"

UGameUIPopLayout::UGameUIPopLayout(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PopName = "";
}

void UGameUIPopLayout::InitPopLayout(const FPopLayoutParam& PopLayoutParam)
{
	PopName = PopLayoutParam.PopName;
}

UGameUIPanel* UGameUIPopLayout::GetParentPanel()
{
	return ParentPanel;
}

void UGameUIPopLayout::SetParentPanel(UGameUIPanel* Panel)
{
	ParentPanel = Panel;
}

void UGameUIPopLayout::Dispose()
{
	this->RemoveFromParent();
}

UUserWidget* UGameUIPopLayout::SetContent(TSubclassOf<UUserWidget> ContentWidgetClass)
{
	if (ContentWidgetClass == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPopLayout SetContent Error, ContentWidgetClass is NULL"));
		return nullptr;
	}

	UPanelWidget* ContentPanel = Cast<UPanelWidget>(this->WidgetTree->FindWidget("ContentPanel"));
	if (ContentPanel == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPopLayout SetContent Error, Cannot find ContentPanel"));
		return nullptr;
	}

	ContentWidget = CreateWidget(GetWorld(), ContentWidgetClass);
	if (ContentWidget == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPopLayout SetContent Error, Create PopLayout Content Widget return NULL"));
		return nullptr;
	}

	ContentPanel->AddChild(ContentWidget);
	UGameUIPanel::FullFillWidget(ContentWidget);

	return ContentWidget;
}

UUserWidget* UGameUIPopLayout::GetContent()
{
	return ContentWidget;
}

UGameUIPopLayout* UGameUIPopLayout::GetPopLayoutByContent(UUserWidget* ContentWidget)
{
	if (ContentWidget == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPopLayout GetPopLayoutByContent Error, ContentWidget is NULL"));
		return nullptr;
	}

	UPanelWidget* PanelWidget = ContentWidget->GetParent();
	if (PanelWidget == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPopLayout GetPopLayoutByContent Error, ContentWidget has no Panrent"));
		return nullptr;
	}

	UObject* WidgetTreeObject = PanelWidget->GetOuter();
	if (WidgetTreeObject == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPopLayout GetPopLayoutByContent Error, Cannot get WidgetTree"));
		return nullptr;
	}

	UObject* PopLayoutObject = WidgetTreeObject->GetOuter();
	if (PopLayoutObject == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPopLayout GetPopLayoutByContent Error, Cannot Get PopLayoutObject"));
		return nullptr;
	}

	return  Cast<UGameUIPopLayout>(PopLayoutObject);
}