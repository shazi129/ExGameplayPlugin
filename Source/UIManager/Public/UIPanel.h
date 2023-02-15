#pragma once

#include "Blueprint/UserWidget.h"
#include "UIPopLayout.h"
#include "UIPanel.generated.h"

//交互行为类型
UENUM(BlueprintType)
enum EUIPaneCacheType
{
	E_NO_CACHE = 0				   UMETA(DisplayName = "No Cache"),  //没有缓存
	E_FOREVER_CACHE               UMETA(DisplayName = "Forever"), //永久缓存
};

UCLASS(BlueprintType)
class UIMANAGER_API UUIPanel : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	int SetPanel(TSubclassOf<UUserWidget> contentCls);

	UUIPopLayout* ShowPopLayout(TSubclassOf<UUserWidget> PopClass, UPARAM(ref) FPopLayoutParam& PopParam);

	void RemoveAllPop();

	static void FullFillWidget(UUserWidget* Widget);

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUIPopLayout> PopTemplateClass;

	UPROPERTY(BlueprintReadOnly)
		FName PanelName;

	//上一次展示的Panel名
	UPROPERTY(BlueprintReadOnly)
		FName PreviousPanelName;

	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<UUserWidget> PreviousPanelClass;
	
	UPROPERTY(BlueprintReadOnly)
		UUserWidget* PanelContent;

	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EUIPaneCacheType> CacheType;

private:
	UPanelWidget * GetPopLayoutPanel();

private:
	UPanelWidget* PopLayoutPanel; //all pop widgets' parent

};