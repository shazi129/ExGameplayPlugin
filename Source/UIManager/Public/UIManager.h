#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "UIPanel.h"
#include "UIPopLayout.h"
#include "UIManager.generated.h"


UCLASS(BlueprintType)
class UIMANAGER_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OnWorldTearingDown(UWorld* World);

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UUIManager* GetUIManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
		void SetPanelTemplate(TSubclassOf<UUIPanel> panelTemplate);

	UFUNCTION(BlueprintCallable)
		UUIPanel* FindOrCreateUIPanel(TSubclassOf<UUserWidget> widgetClass, FName panelName, EUIPaneCacheType cacheType = E_FOREVER_CACHE);

	UFUNCTION(BlueprintCallable)
		UUIPanel* FindUIPanelByContentWidget(UUserWidget* content);

	UFUNCTION(BlueprintCallable)
		UUIPanel* ShowPanel(TSubclassOf<UUserWidget> widgetCls, FName panelName, EUIPaneCacheType cacheType = E_FOREVER_CACHE);

	UFUNCTION(BlueprintCallable)
		UUIPanel* ShowPanelObject(UUIPanel* panel);

	UFUNCTION(BlueprintCallable)
		void RemovePanelObject(UUIPanel* panel);

	UFUNCTION(BlueprintCallable)
		void RemoveCurrentPanel();

	UFUNCTION(BlueprintCallable)
		void HideCurrentPanel();

	UFUNCTION(BlueprintCallable)
		void ShowCurrentPanel();

	UFUNCTION(BlueprintCallable)
		bool HasPreviousPanel();

	UFUNCTION(BlueprintCallable)
		UUIPanel* ShowPrevioursPanel();

	UFUNCTION(BlueprintCallable)
		UUIPopLayout* ShowPopLayout(TSubclassOf<UUserWidget> widgetCls, FPopLayoutParam PopParam);

private:
	UUIPanel* CurrentPanel;

	UPROPERTY()
	TMap<FName, UUIPanel*> PanelCache;

	UPROPERTY()
		TMap<UWorld*, UUIPanel*> PanelCache2; 

	TSubclassOf<UUIPanel> PanelTemplateCls;
};
