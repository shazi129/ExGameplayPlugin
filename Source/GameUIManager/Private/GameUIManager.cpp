#include "GameUIManager.h"
#include "GameUIManagerModule.h"
#include "GameDelegates.h"
#include "Kismet/GameplayStatics.h"

void UGameUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UGameUIManager::OnWorldTearingDown);
}

void UGameUIManager::OnWorldTearingDown(UWorld* World)
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

void UGameUIManager::Deinitialize()
{
	Super::Deinitialize();

}

UGameUIManager* UGameUIManager::GetGameUIManager(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UGameUIManager>();
}

void UGameUIManager::SetPanelTemplate(TSubclassOf<UGameUIPanel> panelTemplate)
{
	PanelTemplateCls = panelTemplate;
}

UGameUIPanel* UGameUIManager::FindOrCreateUIPanel(TSubclassOf<UUserWidget> widgetClass, FName panelName, EUIPaneCacheType cacheType)
{
	//���û�����ã��ͼ���һ��Ĭ��ģ��
	if (PanelTemplateCls == nullptr)
	{
		FString defaultPath = TEXT("Blueprint'/ExGameplayPlugin/Template/WB_PanelTemplate.WB_PanelTemplate_C'");
		UClass* defaultCls = LoadClass<UGameUIPanel>(this, *defaultPath);
		if (defaultCls != nullptr)
		{
			PanelTemplateCls = defaultCls;
		}
		else
		{
			UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIManager FindOrCreateUIPanel Error, Panel Template Class is NULL"));
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
		UE_LOG(LogGameUIManager, Warning, TEXT("---UGameUIManager FindOrCreateUIPanel Error, Cannot get panel name"));
		return nullptr;
	}

	UGameUIPanel* panel = nullptr;

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
		UE_LOG(LogGameUIManager, Error, TEXT("---UGameUIManager FindOrCreateUIPanel, IN Widget Class is none"));
		return nullptr;
	}

	//����ģ�����
	UWidget* widget = CreateWidget(GetWorld(), PanelTemplateCls);
	
	if (widget == nullptr)
	{
		UE_LOG(LogGameUIManager, Error, TEXT("---UGameUIManager CreateUIPanel, CreateWidget return null"));
		return nullptr;
	}
	panel = Cast<UGameUIPanel>(widget);
	if (panel == nullptr)
	{
		widget = nullptr;
		UE_LOG(LogGameUIManager, Error, TEXT("---UGameUIManager CreateUIPanel, template is not a UGameUIPanel"));
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

UGameUIPanel* UGameUIManager::FindUIPanelByContentWidget(UUserWidget* content)
{
	for (TPair<FName, UGameUIPanel*> item : PanelCache)
	{
		if (item.Value->PanelContent == content)
		{
			return item.Value;
		}
	}
	return nullptr;
}

UGameUIPanel* UGameUIManager::ShowPanel(TSubclassOf<UUserWidget> widgetCls, FName panelName, EUIPaneCacheType cacheType)
{
	UGameUIPanel* panel = FindOrCreateUIPanel(widgetCls, panelName, cacheType);
	return ShowPanelObject(panel);
}

UGameUIPanel* UGameUIManager::ShowPanelObject(UGameUIPanel* panel)
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
		CurrentPanel->RemoveFromViewport();
		CurrentPanel->RemoveAllPop();
	}
	CurrentPanel = panel;
	CurrentPanel->AddToViewport();

	return panel;
}

void UGameUIManager::RemovePanelObject(UGameUIPanel* panel)
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
	for (TPair<FName, UGameUIPanel*> item : PanelCache)
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

void UGameUIManager::RemoveCurrentPanel()
{
	if (CurrentPanel != nullptr)
	{
		CurrentPanel->RemoveFromViewport();
		CurrentPanel->RemoveAllPop();
		CurrentPanel = nullptr;
	}
}

void UGameUIManager::HideCurrentPanel()
{
	if (CurrentPanel != nullptr)
	{
		CurrentPanel->RemoveFromViewport();
		CurrentPanel->RemoveAllPop();
	}
}

void UGameUIManager::ShowCurrentPanel()
{
	if (CurrentPanel != nullptr && !CurrentPanel->GetParent())
	{
		CurrentPanel->AddToViewport();
	}
}

bool UGameUIManager::HasPreviousPanel()
{
	if (CurrentPanel != nullptr
		&& CurrentPanel->PreviousPanelClass != nullptr)
	{
		return true;
	}
	return false;
}

UGameUIPanel* UGameUIManager::ShowPrevioursPanel()
{
	if (HasPreviousPanel())
	{
		return ShowPanel(CurrentPanel->PreviousPanelClass, CurrentPanel->PreviousPanelName);
	}
	return nullptr;
}

UGameUIPopLayout* UGameUIManager::ShowPopLayout(TSubclassOf<UUserWidget> widgetCls, FPopLayoutParam popParam)
{
	if (CurrentPanel != nullptr)
	{
		return CurrentPanel->ShowPopLayout(widgetCls, popParam);
	}
	return nullptr;
}