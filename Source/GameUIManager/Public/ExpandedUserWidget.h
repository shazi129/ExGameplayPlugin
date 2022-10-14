#pragma once


#include "Blueprint/UserWidget.h"
#include "ExpandedUserWidgetProxy.h"
#include "ExpandedUserWidget.generated.h"

UCLASS(BlueprintType)
class GAMEUIMANAGER_API UExpandedUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UExpandedUserWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
		void SetInteractorActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AActor* GetInteractActor();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Expanded Config")
		UExpandedUserWidgetProxy* Proxy;

private:
	UPROPERTY()
	AActor* InteractActor;

public:
	//////////////////导出到Proxy中的方法//////////////////
	virtual bool Initialize();

	virtual void NativeOnInitialized();
	virtual void NativePreConstruct();
	virtual void NativeConstruct();
	virtual void NativeDestruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);
	virtual void OnWidgetRebuilt() override;

	///////mouse event
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
};