#pragma once

#include "Components/SlateWrapperTypes.h"
#include "GameUIBPLibrary.generated.h"

//Êó±êµÄ¼üÎ»
UENUM(Blueprintable)
enum class EBPMouseButton : uint8
{
	None = 0					UMETA(DisplayName = "None"),
	LeftMouseButton = 1		UMETA(DisplayName = "LeftMouseButton"),
	MiddleMouseButton = 2		UMETA(DisplayName = "MiddleMouseButton"),
	RightMouseButton = 3		UMETA(DisplayName = "RightMouseButton"),

	ThumbMouseButton = 4		UMETA(DisplayName = "ThumbMouseButton"),
	ThumbMouseButton2 = 4		UMETA(DisplayName = "ThumbMouseButton2"),
};


UCLASS()
class UGameUIBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UGameUIBPLibrary")
		static FEventReply CreateEventReplay(bool IsHandled);

	UFUNCTION(BlueprintCallable, Category = "UGameUIBPLibrary")
		static FVector2D GetScreenSpacePositionFromEvent(const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintCallable, Category = "UGameUIBPLibrary")
		static EBPMouseButton GetMousetButtonType(const FName& MouseButtonName);

	UFUNCTION(BlueprintCallable, Category = "UGameUIBPLibrary")
		static EBPMouseButton GetEffectingMouseButton(const FPointerEvent & InMouseEvent);

	UFUNCTION(BlueprintCallable, Category = "UGameUIBPLibrary")
		static TArray<EBPMouseButton> GetPressedButtons(const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintCallable, Category = "UGameUIBPLibrary")
		static FVector2D GetCursorDeltaFromEvent(const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintCallable, Category = "UGameUIBPLibrary")
		static FVector2D GetWheelDeltaFromEvent(const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintCallable, Category = "UGameUIBPLibrary")
		static FVector2D GetGeometrySize(const FGeometry& InGeometry);
};