#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMGLibrary.generated.h"

UCLASS()
class EXUMG_API UUMGLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UMGLibrary")
	static UUserWidget* CreateSubWidgetForPanel(UPanelWidget* Panel, TSubclassOf<UUserWidget> SubWidgetClass, FName SubWidgetName=NAME_None);

	UFUNCTION(BlueprintCallable, Category = "UMGLibrary")
	static void FullFillCanvasWidget(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "UMGLibrary")
	static void SetAnchorCenter(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "UMGLibrary")
	static void SetWidgetSize(UWidget* Widget, FVector2D Size);

	UFUNCTION(BlueprintCallable, Category = "UMGLibrary")
	static void SetAnchorButtomCenter(UWidget* Widget);

	UFUNCTION(BlueprintPure, Category = "UMGLibrary")
	static FVector2D GetWidgetLocalSize(UWidget* Widget);

	UFUNCTION(BlueprintPure, Category = "UMGLibrary")
	static FVector2D GetWidgetAbsoluteSize(UWidget* Widget);

	//清除所有UI
	UFUNCTION(BlueprintCallable, Category = "UMGLibrary", meta = (DefaultToSelf = "WorldContextObject"))
	static void RemoveAllViewportWidgets(const UObject* WorldContextObject);

	//
	UFUNCTION(BlueprintPure, Category = "UMGLibrary", meta = (DefaultToSelf = "WorldContextObject"))
	static FVector2D ConvertLocalPositionToScreen(APlayerController* PlayerController, const FVector2D& LocalPosition, const FVector2D& ScreenLocalSize);
};
