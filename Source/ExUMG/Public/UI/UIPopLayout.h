#pragma once

#include "Blueprint/UserWidget.h"
#include "UIPopLayout.generated.h"

USTRUCT(BlueprintType)
struct EXUMG_API FPopLayoutParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FName PopName;

	UPROPERTY(BlueprintReadWrite)
		bool MaskShadow;

	UPROPERTY(BlueprintReadWrite)
		bool HideWhenNewPop;
};

class UUIPanel;

UCLASS(BlueprintType)
class EXUMG_API UUIPopLayout : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UUIPopLayout* GetPopLayoutByContent(UUserWidget* ContentWidget);

public:
	UFUNCTION(BlueprintCallable)
		UUIPanel* GetParentPanel();

	UFUNCTION(BlueprintCallable)
		void Dispose();

	UFUNCTION(BlueprintCallable)
		UUserWidget* GetContent();

public:
	void InitPopLayout(const FPopLayoutParam& PopLayoutParam);

	void SetParentPanel(UUIPanel* Panel);

	UUserWidget* SetContent(TSubclassOf<UUserWidget> ContentWidgetClass);

public:
	UPROPERTY(EditAnywhere)
		FName PopName;

private:
	UUIPanel* ParentPanel;

	UUserWidget* ContentWidget;
};