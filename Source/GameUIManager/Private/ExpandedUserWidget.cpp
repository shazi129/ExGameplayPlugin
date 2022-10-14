#include "ExpandedUserWidget.h"
#include "GameUIManagerModule.h"

UExpandedUserWidget::UExpandedUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UExpandedUserWidget::SetInteractorActor(AActor* Actor)
{
	InteractActor = Actor;
	if (Proxy != nullptr)
	{
		Proxy->OnInteractActorSet(Actor);
	}
}

AActor* UExpandedUserWidget::GetInteractActor()
{
	return InteractActor;
}

bool UExpandedUserWidget::Initialize()
{
	if (Proxy != nullptr)
	{
		Proxy->SetTarget(this);
		Proxy->Initialize();
	}
	return Super::Initialize();
}

void UExpandedUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Proxy != nullptr)
	{
		Proxy->NativeOnInitialized();
	}
}
void UExpandedUserWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Proxy != nullptr)
	{
		Proxy->NativePreConstruct();
	}
}

void UExpandedUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Proxy != nullptr)
	{
		Proxy->NativeConstruct();
	}
}

void UExpandedUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (Proxy != nullptr)
	{
		Proxy->NativeDestruct();
	}
}

void UExpandedUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Proxy != nullptr)
	{
		Proxy->NativeTick(MyGeometry, InDeltaTime);
	}
}

void UExpandedUserWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
	if (Proxy != nullptr)
	{
		Proxy->NativeOnLevelRemovedFromWorld(InLevel, InWorld);
	}
}
void UExpandedUserWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();
	if (Proxy != nullptr)
	{
		Proxy->NativeOnWidgetRebuilt();
	}
}

FReply UExpandedUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Replay = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (Replay.IsEventHandled() || Proxy == nullptr)
	{
		return Replay;
	}
	return Proxy->NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UExpandedUserWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Replay = Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	if (Replay.IsEventHandled() || Proxy == nullptr)
	{
		return Replay;
	}
	return Proxy->NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UExpandedUserWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Replay = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	if (Replay.IsEventHandled() || Proxy == nullptr)
	{
		return Replay;
	}
	return Proxy->NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UExpandedUserWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetPressedButtons().Num() > 0)
	{
		UE_LOG(LogGameUIManager, Log, TEXT("---UGameUIManager GetGameUIManager error, WorldContextObject is null"));
	}

	FReply Replay = Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	if (Replay.IsEventHandled() || Proxy == nullptr)
	{
		return Replay;
	}
	return Proxy->NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UExpandedUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (Proxy != nullptr)
	{
		Proxy->NativeOnMouseEnter(InGeometry, InMouseEvent);
	}
}
void UExpandedUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (Proxy != nullptr)
	{
		Proxy->NativeOnMouseLeave(InMouseEvent);
	}
}

FReply UExpandedUserWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Replay = Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	if (Replay.IsEventHandled() || Proxy == nullptr)
	{
		return Replay;
	}
	return Proxy->NativeOnMouseWheel(InGeometry, InMouseEvent);
}

FReply UExpandedUserWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Replay = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (Replay.IsEventHandled() || Proxy == nullptr)
	{
		return Replay;
	}
	return Proxy->NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}