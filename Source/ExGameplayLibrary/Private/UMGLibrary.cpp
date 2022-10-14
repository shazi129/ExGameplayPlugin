#include "UMGLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "ExGameplayLibraryModule.h"

UUserWidget* UUMGLibrary::CreateSubWidgetForPanel(UPanelWidget* Panel, TSubclassOf<UUserWidget> SubWidgetClass, FName SubWidgetName)
{
	if (Panel == nullptr)
	{
		EXLIBRARY_LOG(Error, TEXT("UUMGLibrary::CreateSubWidgetForPanel error, Panel is null"));
		return nullptr;
	}

	UUserWidget* UserWidget = CreateWidget(Panel, SubWidgetClass, SubWidgetName);
	if (UserWidget == nullptr)
	{
		EXLIBRARY_LOG(Error, TEXT("UUMGLibrary::CreateSubWidgetForPanel error, CreateWidget failed"));
		return nullptr;
	}

	Panel->AddChild(UserWidget);
	return UserWidget;
}

void UUMGLibrary::FullFillCanvasWidget(UWidget* Widget)
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

void UUMGLibrary::SetAnchorCenter(UWidget* Widget)
{
	if (Widget && Widget->Slot != nullptr)
	{
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
		if (CanvasPanelSlot != nullptr)
		{
			FAnchors anchors(0.5f, 0.5f, 0.5f, 0.5f);
			CanvasPanelSlot->SetAnchors(anchors);

			CanvasPanelSlot->SetAlignment(FVector2D(0.5,0.5));
		}
	}
}

FVector2D UUMGLibrary::GetWidgetLocalSize(UWidget* Widget)
{
	if (Widget == nullptr)
	{
		EXLIBRARY_LOG(Error, TEXT("UUMGLibrary::GetWidgetLocalSize error, Widget is null"));
		return FVector2D(0, 0);
	}
	const FGeometry& Geometry =  Widget->GetTickSpaceGeometry();
	return USlateBlueprintLibrary::GetLocalSize(Geometry);
}

FVector2D UUMGLibrary::GetWidgetAbsoluteSize(UWidget* Widget)
{
	if (Widget == nullptr)
	{
		EXLIBRARY_LOG(Error, TEXT("UUMGLibrary::GetWidgetAbsoluteSize error, Widget is null"));
		return FVector2D(0, 0);
	}
	const FGeometry& Geometry = Widget->GetTickSpaceGeometry();
	return USlateBlueprintLibrary::GetAbsoluteSize(Geometry);
}