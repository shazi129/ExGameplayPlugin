#pragma once

#include "Extensions/UserWidgetExtension.h"
#include "ExUserWidgetExtension.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UExUserWidgetExtension : public UUserWidgetExtension
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	static UUserWidgetExtension* GetUserWidgetExtensionByName(UUserWidget* UserWidget, FName ExtensionName);

public:
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
};