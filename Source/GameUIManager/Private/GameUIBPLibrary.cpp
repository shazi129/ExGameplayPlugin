
#include "GameUIBPLibrary.h"

FEventReply UGameUIBPLibrary::CreateEventReplay(bool IsHandled)
{
	return FEventReply(IsHandled);
}

FVector2D UGameUIBPLibrary::GetScreenSpacePositionFromEvent(const FPointerEvent& InMouseEvent)
{
	return InMouseEvent.GetScreenSpacePosition();
}

EBPMouseButton UGameUIBPLibrary::GetMousetButtonType(const FName& MouseButtonName)
{
	if (MouseButtonName == "LeftMouseButton")
	{
		return EBPMouseButton::LeftMouseButton;
	}
	else if (MouseButtonName == "MiddleMouseButton")
	{
		return EBPMouseButton::MiddleMouseButton;
	}
	else if (MouseButtonName == "RightMouseButton")
	{
		return EBPMouseButton::RightMouseButton;
	}
	else if (MouseButtonName == "ThumbMouseButton")
	{
		return EBPMouseButton::ThumbMouseButton;
	}
	else if (MouseButtonName == "ThumbMouseButton2")
	{
		return EBPMouseButton::ThumbMouseButton2;
	}
	else
	{
		return EBPMouseButton::None;
	}
}

EBPMouseButton UGameUIBPLibrary::GetEffectingMouseButton(const FPointerEvent& InMouseEvent)
{
	FName ButtonName = InMouseEvent.GetEffectingButton().GetFName();
	return GetMousetButtonType(ButtonName);
}

TArray<EBPMouseButton> UGameUIBPLibrary::GetPressedButtons(const FPointerEvent& InMouseEvent)
{
	TArray<EBPMouseButton> Result;

	const TSet<FKey>& PressedButtons = InMouseEvent.GetPressedButtons();
	for (const FKey& ButtonInfo : PressedButtons)
	{
		EBPMouseButton MouseButtonType = GetMousetButtonType(ButtonInfo.GetFName());
		Result.Add(MouseButtonType);
	}
	return Result;
}

FVector2D UGameUIBPLibrary::GetCursorDeltaFromEvent(const FPointerEvent& InMouseEvent)
{
	return InMouseEvent.GetCursorDelta();
}

FVector2D UGameUIBPLibrary::GetWheelDeltaFromEvent(const FPointerEvent& InMouseEvent)
{
	return InMouseEvent.GetGestureDelta();
}

FVector2D UGameUIBPLibrary::GetGeometrySize(const FGeometry& InGeometry)
{
	return InGeometry.Size;
}