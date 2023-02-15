#include "UIManager.h"
#include "UIManagerModule.h"
#include "GameDelegates.h"
#include "Kismet/GameplayStatics.h"

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UUIManager::OnWorldTearingDown);
}

void UUIManager::OnWorldTearingDown(UWorld* World)
{
	if (World == GetWorld())
	{
		if (CurrentPanel != nullptr)
		{
			RemoveCurrentPanel();
		}
		PanelCache.Empty();
	}
}

void UUIManager::Deinitialize()
{
	Super::Deinitialize();

}

UUIManager* UUIManager::GetUIManager(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UUIManager>();
}

void UUIManager::SetPanelTemplate(TSubclassOf<UUIPanel> panelTemplate)
{
	PanelTemplateCls = panelTemplate;
}

UUIPanel* UUIManager::FindOrCreateUIPanel(TSubclassOf<UUserWidget> widgetClass, FName panelName, EUIPaneCacheType cacheType)
{
	//如果没有设置，就加载一个默认模板
	if (PanelTemplateCls == nullptr)
	{
		FString defaultPath = TEXT("Blueprint'/ExGameplayPlugin/Template/WB_PanelTemplate.WB_PanelTemplate_C'");
		UClass* defaultCls = LoadClass<UUIPanel>(this, *defaultPath);
		if (defaultCls != nullptr)
		{
			PanelTemplateCls = defaultCls;
		}
		else
		{
			UIMANAGER_LOG(Warning, TEXT("---UUIManager FindOrCreateUIPanel Error, Panel Template Class is NULL"));
			return nullptr;
		}
	}

	
	if (panelName.IsNone())
	{
		if (widgetClass != nullptr)
		{
			panelName = widgetClass->GetFName();
		}
	}
	if (panelName.IsNone())
	{
		UIMANAGER_LOG(Warning, TEXT("---UUIManager FindOrCreateUIPanel Error, Cannot get panel name"));
		return nullptr;
	}

	UUIPanel* panel = nullptr;

	//先通过名字查找
	if (PanelCache.Contains(panelName))
	{
		panel = PanelCache[panelName];

		//可能存在在外部被干掉的情况
		if (panel == nullptr || panel->CacheType == E_NO_CACHE || panel->PanelName.IsNone()
			|| panel->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed))
		{
			PanelCache.Remove(panelName);
			panel = nullptr;
		}
	}
	if (panel)
	{
		return panel;
	}

	//没找到，就要开始创建了
	if (widgetClass == nullptr)
	{
		UIMANAGER_LOG(Error, TEXT("---UUIManager FindOrCreateUIPanel, IN Widget Class is none"));
		return nullptr;
	}

	//创建模板对象
	UWidget* widget = CreateWidget(GetWorld(), PanelTemplateCls);
	
	if (widget == nullptr)
	{
		UIMANAGER_LOG(Error, TEXT("---UUIManager CreateUIPanel, CreateWidget return null"));
		return nullptr;
	}
	panel = Cast<UUIPanel>(widget);
	if (panel == nullptr)
	{
		widget = nullptr;
		UIMANAGER_LOG(Error, TEXT("---UUIManager CreateUIPanel, template is not a UUIPanel"));
		return nullptr;
	}

	//panel的名字
	panel->PanelName = panelName;
	panel->CacheType = cacheType;

	//添加内容
	int result = panel->SetPanel(widgetClass);
	if (result != 0) //如果添加失败，删除已创建的模板
	{
		panel->RemoveFromParent();
		panel = nullptr;
		return nullptr;
	}

	//加入缓存
	if (panel->CacheType != E_NO_CACHE)
	{
		PanelCache.Add(panelName, panel);
	}
	return panel;
}

UUIPanel* UUIManager::FindUIPanelByContentWidget(UUserWidget* content)
{
	for (TPair<FName, UUIPanel*> item : PanelCache)
	{
		if (item.Value->PanelContent == content)
		{
			return item.Value;
		}
	}
	return nullptr;
}

UUIPanel* UUIManager::ShowPanel(TSubclassOf<UUserWidget> widgetCls, FName panelName, EUIPaneCacheType cacheType)
{
	UUIPanel* panel = FindOrCreateUIPanel(widgetCls, panelName, cacheType);
	return ShowPanelObject(panel);
}

UUIPanel* UUIManager::ShowPanelObject(UUIPanel* panel)
{
	if (panel == nullptr)
	{
		return nullptr;
	}

	if (panel == CurrentPanel)
	{
		ShowCurrentPanel();
		return panel;
	}

	if (CurrentPanel && CurrentPanel->PanelContent)
	{
		panel->PreviousPanelName = CurrentPanel->PanelName;
		panel->PreviousPanelClass = CurrentPanel->PanelContent->GetClass();
		CurrentPanel->RemoveFromParent();
		CurrentPanel->RemoveAllPop();
	}
	CurrentPanel = panel;
	CurrentPanel->AddToViewport();

	return panel;
}

void UUIManager::RemovePanelObject(UUIPanel* panel)
{
	if (panel == nullptr)
	{
		return;
	}

	if (panel == CurrentPanel)
	{
		RemoveCurrentPanel();
	}

	//在cache中找到，删掉它
	FName panelName = "";
	for (TPair<FName, UUIPanel*> item : PanelCache)
	{
		if (item.Value->PanelContent == panel)
		{
			panelName = item.Key;
			break;
		}
	}

	if (!panelName.IsNone())
	{
		PanelCache.Remove(panelName);
	}
}

void UUIManager::RemoveCurrentPanel()
{
	if (CurrentPanel != nullptr)
	{
		CurrentPanel->RemoveFromParent();
		CurrentPanel->RemoveAllPop();
		CurrentPanel = nullptr;
	}
}

void UUIManager::HideCurrentPanel()
{
	if (CurrentPanel != nullptr)
	{
		CurrentPanel->RemoveFromParent();
		CurrentPanel->RemoveAllPop();
	}
}

void UUIManager::ShowCurrentPanel()
{
	if (CurrentPanel != nullptr && !CurrentPanel->GetParent())
	{
		CurrentPanel->AddToViewport();
	}
}

bool UUIManager::HasPreviousPanel()
{
	if (CurrentPanel != nullptr
		&& CurrentPanel->PreviousPanelClass != nullptr)
	{
		return true;
	}
	return false;
}

UUIPanel* UUIManager::ShowPrevioursPanel()
{
	if (HasPreviousPanel())
	{
		return ShowPanel(CurrentPanel->PreviousPanelClass, CurrentPanel->PreviousPanelName);
	}
	return nullptr;
}

UUIPopLayout* UUIManager::ShowPopLayout(TSubclassOf<UUserWidget> widgetCls, FPopLayoutParam popParam)
{
	if (CurrentPanel != nullptr)
	{
		return CurrentPanel->ShowPopLayout(widgetCls, popParam);
	}
	return nullptr;
}