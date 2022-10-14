#include "GameUIPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "GameUIManagerModule.h"

UGameUIPanel::UGameUIPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PanelName = "";
}

void UGameUIPanel::FullFillWidget(UUserWidget* Widget)
{
	if (Widget && Widget->Slot != nullptr)
	{
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
		if (CanvasPanelSlot != nullptr)
		{
			FAnchors anchors(0.0f, 0.0f, 1.0f, 1.0f);
			CanvasPanelSlot->SetAnchors(anchors);

			FMargin margin;
			CanvasPanelSlot->SetOffsets(margin);
		}
	}
}

int UGameUIPanel::SetPanel(TSubclassOf<UUserWidget> contentCls)
{
	UWidget* panel = this->WidgetTree->FindWidget("UIPanel");
	if (panel == nullptr)
	{
		return -1;
	}
	UPanelWidget* panelWidget = Cast<UPanelWidget>(panel);
	if (panelWidget == nullptr)
	{
		return -1;
	}

	UWidget* widget = CreateWidget(panelWidget, contentCls);
	if (widget == nullptr)
	{
		return -1;
	}

	UUserWidget* contentWidget = Cast<UUserWidget>(widget);
	if (contentWidget == nullptr)
	{
		return -1;
	}

	if (!PanelName.IsValid())
	{
		PanelName = contentCls->GetFName();
	}
	PanelContent = contentWidget;

	PanelContent->SetOwningPlayer(GetOwningPlayer());
	panelWidget->AddChild(PanelContent);
	FullFillWidget(PanelContent);

	return 0;
}

UGameUIPopLayout* UGameUIPanel::ShowPopLayout(TSubclassOf<UUserWidget> PopClass, UPARAM(ref) FPopLayoutParam& PopParam)
{
	if (PopClass == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPanel ShowPopLayout Error, Pop Class is NULL"));
		return nullptr;
	}

	UPanelWidget* LayoutPanel = GetPopLayoutPanel();
	if (LayoutPanel == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPanel ShowPopLayout Error, Tempalte[%s] has no UIPoplayout Layer"), *this->GetName());
		return nullptr;
	}

	//如果没有设置，就加载一个默认模板
	if (PopTemplateClass == nullptr)
	{
		FString defaultPath = TEXT("Blueprint'/ExGameplayPlugin/Template/WB_PopTemplate.WB_PopTemplate_C'");
		UClass* defaultCls = LoadClass<UGameUIPopLayout>(this, *defaultPath);
		if (defaultCls != nullptr)
		{
			PopTemplateClass = defaultCls;
		}
		else
		{
			UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPanel ShowPopLayout Error, Pop Template Class is NULL"));
			return nullptr;
		}
	}

	if (PopParam.PopName.IsNone())
	{
		PopParam.PopName = PopClass->GetFName();
	}

	UUserWidget* Widget = CreateWidget(GetWorld(), PopTemplateClass);
	if (Widget == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPanel ShowPopLayout Error, Create PopLayout Widget return NULL"));
		return nullptr;
	}

	UGameUIPopLayout* PoplayoutWidget = Cast<UGameUIPopLayout>(Widget);
	if (PoplayoutWidget == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPanel ShowPopLayout Error, PopTemplateClass is not a UGameUIPopLayout"));
		return nullptr;
	}

	LayoutPanel->AddChild(PoplayoutWidget);
	PoplayoutWidget->SetParentPanel(this);
	FullFillWidget(PoplayoutWidget);
	PoplayoutWidget->InitPopLayout(PopParam);
	PoplayoutWidget->SetContent(PopClass);

	return PoplayoutWidget;
}

UPanelWidget* UGameUIPanel::GetPopLayoutPanel()
{
	if (PopLayoutPanel == nullptr)
	{
		PopLayoutPanel = Cast<UPanelWidget>(this->WidgetTree->FindWidget("PopList"));
	}
	return PopLayoutPanel;
}

void UGameUIPanel::RemoveAllPop()
{
	UPanelWidget* LayoutPanel = GetPopLayoutPanel();
	if (LayoutPanel == nullptr)
	{
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIPanel RemoveAllPop Error, Tempalte[%s] has no UIPoplayout Layer"), *this->GetName());
		return;
	}

	int ChildrenCount = LayoutPanel->GetChildrenCount();
	for (int i = ChildrenCount - 1; i >= 0; i--)
	{
		LayoutPanel->RemoveChildAt(i);
	}
}