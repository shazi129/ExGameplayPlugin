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
	//���û�����ã��ͼ���һ��Ĭ��ģ��
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

	//��ͨ�����ֲ���
	if (PanelCache.Contains(panelName))
	{
		panel = PanelCache[panelName];

		//���ܴ������ⲿ���ɵ������
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

	//û�ҵ�����Ҫ��ʼ������
	if (widgetClass == nullptr)
	{
		UIMANAGER_LOG(Error, TEXT("---UUIManager FindOrCreateUIPanel, IN Widget Class is none"));
		return nullptr;
	}

	//����ģ�����
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

	//panel������
	panel->PanelName = panelName;
	panel->CacheType = cacheType;

	//�������
	int result = panel->SetPanel(widgetClass);
	if (result != 0) //������ʧ�ܣ�ɾ���Ѵ�����ģ��
	{
		panel->RemoveFromParent();
		panel = nullptr;
		return nullptr;
	}

	//���뻺��
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

	//��cache���ҵ���ɾ����
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