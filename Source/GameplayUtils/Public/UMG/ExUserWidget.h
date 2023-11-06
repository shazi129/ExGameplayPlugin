#pragma once

#include "Blueprint/UserWidget.h"
#include "ExUserWidget.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UExUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Extensions")
	TArray<TSoftClassPtr<UUserWidgetExtension>> ExtensionClasses;

protected:
	virtual void NativeOnInitialized() override;
};