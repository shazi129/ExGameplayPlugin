#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameUIPanel.h"
#include "GameUIPopLayout.h"
#include "GameUIManager.generated.h"


UCLASS(BlueprintType)
class GAMEUIMANAGER_API UGameUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OnWorldTearingDown(UWorld* World);

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UGameUIManager* GetGameUIManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
		void SetPanelTemplate(TSubclassOf<UGameUIPanel> panelTemplate);

	UFUNCTION(BlueprintCallable)
		UGameUIPanel* FindOrCreateUIPanel(TSubclassOf<UUserWidget> widgetClass, FName panelName, EUIPaneCacheType cacheType = E_FOREVER_CACHE);

	UFUNCTION(BlueprintCallable)
		UGameUIPanel* FindUIPanelByContentWidget(UUserWidget* content);

	UFUNCTION(BlueprintCallable)
		UGameUIPanel* ShowPanel(TSubclassOf<UUserWidget> widgetCls, FName panelName, EUIPaneCacheType cacheType = E_FOREVER_CACHE);

	UFUNCTION(BlueprintCallable)
		UGameUIPanel* ShowPanelObject(UGameUIPanel* panel);

	UFUNCTION(BlueprintCallable)
		void RemovePanelObject(UGameUIPanel* panel);

	UFUNCTION(BlueprintCallable)
		void RemoveCurrentPanel();

	UFUNCTION(BlueprintCallable)
		void HideCurrentPanel();

	UFUNCTION(BlueprintCallable)
		void ShowCurrentPanel();

	UFUNCTION(BlueprintCallable)
		bool HasPreviousPanel();

	UFUNCTION(BlueprintCallable)
		UGameUIPanel* ShowPrevioursPanel();

	UFUNCTION(BlueprintCallable)
		UGameUIPopLayout* ShowPopLayout(TSubclassOf<UUserWidget> widgetCls, FPopLayoutParam PopParam);

private:
	UGameUIPanel* CurrentPanel;

	UPROPERTY()
	TMap<FName, UGameUIPanel*> PanelCache;

	UPROPERTY()
		TMap<UWorld*, UGameUIPanel*> PanelCache2; 

	TSubclassOf<UGameUIPanel> PanelTemplateCls;
};
