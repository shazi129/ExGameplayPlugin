#include "Utils/UMGLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "ExUMGModule.h"
#include "Math/UnrealMathUtility.h"

UWidget* UUMGLibrary::ClearWidget(UWidgetTree* WidgetTree, FString RootName)
{
	if (RootName.IsEmpty() || !WidgetTree)
	{
		return nullptr;
	}

	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);

	UWidget* RootWidget = nullptr;

	for (int i = 0; i < Widgets.Num(); i++)
	{
		UWidget* Widget = Widgets[i];

		if (RootWidget == nullptr && Widget->GetName() == RootName)
		{
			RootWidget = Widget;
			continue;
		}

		if (!Widget->Slot || !Widget->Slot->Parent)
		{
			continue;
		}

		TObjectPtr<UWidget> Parent = Widget->Slot->Parent;
		if (Parent->GetName() == RootName)
		{
			WidgetTree->RemoveWidget(Widget);
		}
	}

	return RootWidget;
}

UUserWidget* UUMGLibrary::CreateSubWidgetForPanel(UPanelWidget* Panel, TSubclassOf<UUserWidget> SubWidgetClass, FName SubWidgetName)
{
	if (Panel == nullptr)
	{
		EXUMG_LOG(Error, TEXT("UUMGLibrary::CreateSubWidgetForPanel error, Panel is null"));
		return nullptr;
	}

	UUserWidget* UserWidget = CreateWidget(Panel, SubWidgetClass, SubWidgetName);
	if (UserWidget == nullptr)
	{
		EXUMG_LOG(Error, TEXT("UUMGLibrary::CreateSubWidgetForPanel error, CreateWidget failed"));
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

void UUMGLibrary::SetWidgetSize(UWidget* Widget, FVector2D Size)
{
	if (Widget && Widget->Slot != nullptr)
	{
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
		if (CanvasPanelSlot != nullptr)
		{
			CanvasPanelSlot->SetSize(Size);
		}
	}
}

void UUMGLibrary::SetAnchorButtomCenter(UWidget* Widget)
{
	if (Widget && Widget->Slot != nullptr)
	{
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
		if (CanvasPanelSlot != nullptr)
		{
			FAnchors anchors(0.5f, 1.0f, 0.5f, 1.0f);
			CanvasPanelSlot->SetAnchors(anchors);

			CanvasPanelSlot->SetAlignment(FVector2D(0.5f, 1.0f));
		}
	}
}

FVector2D UUMGLibrary::GetWidgetLocalSize(UWidget* Widget)
{
	if (Widget == nullptr)
	{
		EXUMG_LOG(Error, TEXT("UUMGLibrary::GetWidgetLocalSize error, Widget is null"));
		return FVector2D(0, 0);
	}
	const FGeometry& Geometry =  Widget->GetTickSpaceGeometry();
	return USlateBlueprintLibrary::GetLocalSize(Geometry);
}

FVector2D UUMGLibrary::GetWidgetAbsoluteSize(UWidget* Widget)
{
	if (Widget == nullptr)
	{
		EXUMG_LOG(Error, TEXT("UUMGLibrary::GetWidgetAbsoluteSize error, Widget is null"));
		return FVector2D(0, 0);
	}
	const FGeometry& Geometry = Widget->GetTickSpaceGeometry();
	return USlateBlueprintLibrary::GetAbsoluteSize(Geometry);
}

void UUMGLibrary::RemoveAllViewportWidgets(const UObject* WorldContextObject)
{
	if (WorldContextObject != nullptr)
	{
		UWorld* World = WorldContextObject->GetWorld();
		if (World && World->IsGameWorld())
		{
			if (UGameViewportClient* ViewportClient = World->GetGameViewport())
			{
				ViewportClient->RemoveAllViewportWidgets();
			}
		}
	}
}

FVector2D UUMGLibrary::ConvertLocalPositionToScreen(APlayerController* PlayerController, const FVector2D& LocalPosition, const FVector2D& ScreenLocalSize)
{
	if (!PlayerController || ScreenLocalSize.X == 0 || ScreenLocalSize.Y == 0)
	{
		return FVector2D(0, 0);
	}

	//实际屏幕大小
	int ScreenSizeX;
	int ScreenSizeY;
	PlayerController->GetViewportSize(ScreenSizeX, ScreenSizeY); 

	//实际大小与设计大小的缩放
	FVector2D Scale = FVector2D(ScreenSizeX / ScreenLocalSize.X, ScreenSizeY / ScreenLocalSize.Y);

	return LocalPosition * Scale;
}

UUserWidget* UUMGLibrary::GetOwningUserWidget(UWidget* Widget)
{
	if (UUserWidget* Result = Cast<UUserWidget>(Widget))
	{
		return Result;
	}
	UPanelWidget* PanelWidget = Cast<UPanelWidget>(Widget);
	if (!PanelWidget)
	{
		PanelWidget = Widget->GetParent();
	}

	if (PanelWidget)
	{
		if (UWidgetTree* WidgetTree = Cast<UWidgetTree>(PanelWidget->GetOuter()))
		{
			return Cast<UUserWidget>(WidgetTree->GetOuter());
		}
		
	}
	return nullptr;
}