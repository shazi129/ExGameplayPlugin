#include "UI/UIPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "ExUMGModule.h"

UUIPanel::UUIPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PanelName = "";
}

void UUIPanel::FullFillWidget(UUserWidget* Widget)
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

int UUIPanel::SetPanel(TSubclassOf<UUserWidget> contentCls)
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

UUIPopLayout* UUIPanel::ShowPopLayout(TSubclassOf<UUserWidget> PopClass, UPARAM(ref) FPopLayoutParam& PopParam)
{
	if (PopClass == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPanel ShowPopLayout Error, Pop Class is NULL"));
		return nullptr;
	}

	UPanelWidget* LayoutPanel = GetPopLayoutPanel();
	if (LayoutPanel == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPanel ShowPopLayout Error, Tempalte[%s] has no UIPoplayout Layer"), *this->GetName());
		return nullptr;
	}

	//如果没有设置，就加载一个默认模板
	if (PopTemplateClass == nullptr)
	{
		FString defaultPath = TEXT("Blueprint'/ExGameplayPlugin/Template/WB_PopTemplate.WB_PopTemplate_C'");
		UClass* defaultCls = LoadClass<UUIPopLayout>(this, *defaultPath);
		if (defaultCls != nullptr)
		{
			PopTemplateClass = defaultCls;
		}
		else
		{
			EXUMG_LOG(Warning, TEXT("---UUIPanel ShowPopLayout Error, Pop Template Class is NULL"));
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
		EXUMG_LOG(Warning, TEXT("---UUIPanel ShowPopLayout Error, Create PopLayout Widget return NULL"));
		return nullptr;
	}

	UUIPopLayout* PoplayoutWidget = Cast<UUIPopLayout>(Widget);
	if (PoplayoutWidget == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPanel ShowPopLayout Error, PopTemplateClass is not a UUIPopLayout"));
		return nullptr;
	}

	LayoutPanel->AddChild(PoplayoutWidget);
	PoplayoutWidget->SetParentPanel(this);
	FullFillWidget(PoplayoutWidget);
	PoplayoutWidget->InitPopLayout(PopParam);
	PoplayoutWidget->SetContent(PopClass);

	return PoplayoutWidget;
}

UPanelWidget* UUIPanel::GetPopLayoutPanel()
{
	if (PopLayoutPanel == nullptr)
	{
		PopLayoutPanel = Cast<UPanelWidget>(this->WidgetTree->FindWidget("PopList"));
	}
	return PopLayoutPanel;
}

void UUIPanel::RemoveAllPop()
{
	UPanelWidget* LayoutPanel = GetPopLayoutPanel();
	if (LayoutPanel == nullptr)
	{
		EXUMG_LOG(Warning, TEXT("---UUIPanel RemoveAllPop Error, Tempalte[%s] has no UIPoplayout Layer"), *this->GetName());
		return;
	}

	int ChildrenCount = LayoutPanel->GetChildrenCount();
	for (int i = ChildrenCount - 1; i >= 0; i--)
	{
		LayoutPanel->RemoveChildAt(i);
	}
}