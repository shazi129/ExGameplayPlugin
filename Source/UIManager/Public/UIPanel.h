#pragma once

#include "Blueprint/UserWidget.h"
#include "UIPopLayout.h"
#include "UIPanel.generated.h"

//������Ϊ����
UENUM(BlueprintType)
enum EUIPaneCacheType
{
	E_NO_CACHE = 0				   UMETA(DisplayName = "No Cache"),  //û�л���
	E_FOREVER_CACHE               UMETA(DisplayName = "Forever"), //���û���
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

	//��һ��չʾ��Panel��
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