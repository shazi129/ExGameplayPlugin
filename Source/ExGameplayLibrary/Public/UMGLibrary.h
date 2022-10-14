#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMGLibrary.generated.h"

UCLASS()
class EXGAMEPLAYLIBRARY_API UUMGLibrary : public UBlueprintFunctionLibrary
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
	static FVector2D GetWidgetLocalSize(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "UMGLibrary")
	static FVector2D GetWidgetAbsoluteSize(UWidget* Widget);
};
