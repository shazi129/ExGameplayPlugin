#pragma  once 

#include "Blueprint/UserWidget.h"
#include "Input/Events.h"
#include "ExpandedUserWidgetProxy.generated.h"

class UExpandedUserWidget;

UCLASS(EditInlineNew, BlueprintType, Blueprintable)
class GAMEUIMANAGER_API UExpandedUserWidgetProxy : public UObject
{
	GENERATED_BODY()

public:
	UExpandedUserWidgetProxy(const FObjectInitializer& ObjectInitializer);

	/// 代理的UserWidget的方法
	virtual bool Initialize();
	virtual void NativeOnInitialized();
	virtual void NativePreConstruct();
	virtual void NativeConstruct();
	virtual void NativeDestruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void NativeOnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);
	virtual void NativeOnWidgetRebuilt();
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

public:
	UFUNCTION(BlueprintCallable)
		UExpandedUserWidget* GetTarget();

	UFUNCTION(BlueprintImplementableEvent)
		void OnWidgetInitialized();

	UFUNCTION(BlueprintImplementableEvent)
		void PreWidgetConstruct();

	UFUNCTION(BlueprintImplementableEvent)
		void WidgetConstruct();

	UFUNCTION(BlueprintImplementableEvent)
		void WidgetDestruct();

	UFUNCTION(BlueprintImplementableEvent)
		void WidgetTick(FGeometry MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
		void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);

	UFUNCTION(BlueprintImplementableEvent)
		void OnWidgetRebuilt();

	UFUNCTION(BlueprintImplementableEvent)
		void OnInteractActorSet(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent)
		FEventReply OnMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION(BlueprintImplementableEvent)
		FEventReply OnPreviewMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION(BlueprintImplementableEvent)
		FEventReply OnMouseButtonUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION(BlueprintImplementableEvent)
		FEventReply OnMouseMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION(BlueprintImplementableEvent)
		void OnMouseEnter(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION(BlueprintImplementableEvent)
		void OnMouseLeave(const FPointerEvent& MouseEvent);

	UFUNCTION(BlueprintImplementableEvent)
		FEventReply OnMouseWheel(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION(BlueprintImplementableEvent)
		FEventReply OnMouseButtonDoubleClick(FGeometry InMyGeometry, const FPointerEvent& InMouseEvent);

public:
	void SetTarget(UExpandedUserWidget* Widget);

protected:
	UPROPERTY()
		UExpandedUserWidget* UserWidget;
};