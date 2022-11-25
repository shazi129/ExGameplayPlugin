#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "WidgetInteractActions/WidgetInteractAction.h"
#include "ExButton.generated.h"

UCLASS()
class EXUMG_API UExButton : public UButton
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnRebuildWidget();

	UFUNCTION(BlueprintCallable)
	void HandleClickActions();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category="Interact Actions")
		TArray<UWidgetInteractAction*> ClickActions;
};