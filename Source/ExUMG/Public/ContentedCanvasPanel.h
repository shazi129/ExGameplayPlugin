#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "ContentedCanvasPanel.generated.h"

USTRUCT(BlueprintType)
struct EXUMG_API FCanvasContent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WidgetName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidget* Widget;
};

UCLASS()
class EXUMG_API UContentedCanvasPanel : public UCanvasPanel
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Content")
	int ID;

	UPROPERTY(EditAnywhere, Category = "Content")
	FName Tag;

	UPROPERTY(EditAnywhere, Category = "Content", meta=(BindWidget))
	TArray<FCanvasContent> Contents;

public:
	UFUNCTION(BlueprintPure)
	UWidget* GetContent(FName Name);

	UFUNCTION(BlueprintPure)
	UWidget* GetChildByClass(TSubclassOf<UWidget> ChildClass);

	UFUNCTION(BlueprintPure)
	TArray<UWidget*> GetAllChildByClass(TSubclassOf<UWidget> ChildClass);
};