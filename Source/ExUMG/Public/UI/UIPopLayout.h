#pragma once

#include "Blueprint/UserWidget.h"
#include "GameUIPopLayout.generated.h"

USTRUCT(BlueprintType)
struct FPopLayoutParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FName PopName;

	UPROPERTY(BlueprintReadWrite)
		bool MaskShadow;

	UPROPERTY(BlueprintReadWrite)
		bool HideWhenNewPop;
};

class UGameUIPanel;

UCLASS(BlueprintType)
class UGameUIPopLayout : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UGameUIPopLayout* GetPopLayoutByContent(UUserWidget* ContentWidget);

public:
	UFUNCTION(BlueprintCallable)
		UGameUIPanel* GetParentPanel();

	UFUNCTION(BlueprintCallable)
		void Dispose();

	UFUNCTION(BlueprintCallable)
		UUserWidget* GetContent();

public:
	void InitPopLayout(const FPopLayoutParam& PopLayoutParam);

	void SetParentPanel(UGameUIPanel* Panel);

	UUserWidget* SetContent(TSubclassOf<UUserWidget> ContentWidgetClass);

public:
	UPROPERTY(EditAnywhere)
		FName PopName;

private:
	UGameUIPanel* ParentPanel;

	UUserWidget* ContentWidget;
};