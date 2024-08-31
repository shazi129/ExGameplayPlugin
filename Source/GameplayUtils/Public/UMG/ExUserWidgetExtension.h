#pragma once

#include "Extensions/UserWidgetExtension.h"
#include "ExUserWidgetExtension.generated.h"

/**
 * @brief 
 * 注意：由于在UserWidget中，创建Extension的方式：
 * UUserWidgetExtension* Extension = NewObject<UUserWidgetExtension>(this, InExtensionType);
 * 没有指定name为None， 可能导致Extension对象复用，所以最好在Dstruct中清除运行时数据
*/

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UExUserWidgetExtension : public UUserWidgetExtension
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	static UUserWidgetExtension* GetUserWidgetExtensionByName(UUserWidget* UserWidget, FName ExtensionName);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	static UUserWidgetExtension* GetWidgetComponentExtensionByClass(class UWidgetComponent* Widget, TSubclassOf<UUserWidgetExtension> Class);

public:
	UExUserWidgetExtension();

	virtual void Initialize();

	virtual void Construct();

	virtual void Destruct();

	virtual bool RequiresTick() const;

	virtual void Tick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, meta = (DisplayName = "Initialize"))
	void ReceiveInitialize();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, meta = (DisplayName = "Construct"))
	void ReceiveConstruct();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, meta = (DisplayName = "Destruct"))
	void ReceiveDestruct();

	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic)
	bool IsTickable() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, meta = (DisplayName = "Tick"))
	void ReceiveTick(FGeometry MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	UUserWidget* GetOuterUserWidget();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetVisibility(ESlateVisibility InVisibility);

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bTickable = true;
};