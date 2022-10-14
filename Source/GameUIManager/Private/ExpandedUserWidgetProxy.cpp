#include "ExpandedUserWidgetProxy.h"
#include "ExpandedUserWidget.h"

UExpandedUserWidgetProxy::UExpandedUserWidgetProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UExpandedUserWidget* UExpandedUserWidgetProxy::GetTarget()
{
	return UserWidget;
}

void UExpandedUserWidgetProxy::SetTarget(UExpandedUserWidget* Widget)
{
	UserWidget = Widget;
}

bool UExpandedUserWidgetProxy::Initialize()
{
	return true;
}

void UExpandedUserWidgetProxy::NativeOnInitialized()
{
	OnWidgetInitialized();
}

void UExpandedUserWidgetProxy::NativePreConstruct()
{
	PreWidgetConstruct();
}

void UExpandedUserWidgetProxy::NativeConstruct()
{
	WidgetConstruct();
}

void UExpandedUserWidgetProxy::NativeDestruct()
{
	WidgetDestruct();
}

void UExpandedUserWidgetProxy::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	WidgetTick(MyGeometry, InDeltaTime);
}

void UExpandedUserWidgetProxy::NativeOnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UExpandedUserWidgetProxy::NativeOnWidgetRebuilt()
{
	OnWidgetRebuilt();
}

FReply UExpandedUserWidgetProxy::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDown(InGeometry, InMouseEvent).NativeReply;
}

FReply UExpandedUserWidgetProxy::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnPreviewMouseButtonDown(InGeometry, InMouseEvent).NativeReply;
}

FReply UExpandedUserWidgetProxy::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonUp(InGeometry, InMouseEvent).NativeReply;
}

FReply UExpandedUserWidgetProxy::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseMove(InGeometry, InMouseEvent).NativeReply;
}

void UExpandedUserWidgetProxy::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnMouseEnter(InGeometry, InMouseEvent);
}

void UExpandedUserWidgetProxy::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnMouseLeave(InMouseEvent);
}

FReply UExpandedUserWidgetProxy::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseWheel(InGeometry, InMouseEvent).NativeReply;
}

FReply UExpandedUserWidgetProxy::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDoubleClick(InGeometry, InMouseEvent).NativeReply;
}
