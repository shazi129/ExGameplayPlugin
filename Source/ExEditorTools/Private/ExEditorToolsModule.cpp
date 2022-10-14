// .cpp
// Copyright Epic Games, Inc. All Rights Reserved.
#include "ExEditorToolsModule.h"
#include "ExEditorToolsStyle.h"
#include "ExEditorToolsCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "ExEditorToolsConfig.h"
#include "ExEditorToolsAction.h"
#include "ISettingsModule.h"

static const FName ExEditorToolsTabName("ExEditorTools");

DEFINE_LOG_CATEGORY(LogExEditorTools);

#define LOCTEXT_NAMESPACE "FExEditorToolsModule"

void FExEditorToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FExEditorToolsStyle::Initialize();
	FExEditorToolsStyle::ReloadTextures();

	FExEditorToolsCommands::Register();

	RegisterSettings();
	
	PluginCommands = MakeShareable(new FUICommandList);

	for(int i=0;i<FExEditorToolsCommands::Get().ConfigCommands.Num();++i)
	{
		PluginCommands->MapAction(
			FExEditorToolsCommands::Get().ConfigCommands[i],
			FExecuteAction::CreateRaw(this, &FExEditorToolsModule::ExecuteToolAction, i),
			FCanExecuteAction());
	}

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FExEditorToolsModule::RegisterMenus));
	
	/*FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ExEditorToolsTabName, FOnSpawnTab::CreateRaw(this, &FExEditorToolsModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FExEditorToolsTabTitle", "ExEditorTools"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);*/
}

void FExEditorToolsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	UnregisterSettings();

	FExEditorToolsStyle::Shutdown();

	FExEditorToolsCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ExEditorToolsTabName);
}

TSharedRef<SDockTab> FExEditorToolsModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FExEditorToolsModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ExEditorTools.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FExEditorToolsModule::ExecuteToolAction(int Index)
{
	UExEditorToolsContext* Context = GetMutableDefault<UExEditorToolsConfig>()->GetToolContext();
	if (Context && 0 <= Index && Index < Context->ToolItems.Num())
	{
		UExEditorActionDefine* Define = Cast<UExEditorActionDefine>(Context->ToolItems[Index].Action.TryLoad());
		if (Define && Define->Action)
		{
			Define->Action->DoAction();
		}
	}
}

TSharedRef<SWidget> FExEditorToolsModule::FillComboButton(TSharedPtr<class FUICommandList> Commands)
{
	FMenuBuilder MenuBuilder(true, Commands);

	const UExEditorToolsConfig* EditorConfig = GetDefault<UExEditorToolsConfig>();
	for (int i = 0; i < FExEditorToolsCommands::Get().ConfigCommands.Num(); ++i)
	{
			MenuBuilder.AddMenuEntry(FExEditorToolsCommands::Get().ConfigCommands[i], NAME_None, TAttribute<FText>(), TAttribute<FText>());
	}

	return MenuBuilder.MakeWidget();
}

void FExEditorToolsModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ExEditorToolsTabName);
}

void FExEditorToolsModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

// 	{
// 		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
// 		{
// 			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
// 			for (int i = 0; i < FExEditorToolsCommands::Get().ConfigCommands.Num(); ++i)
// 			{
// 				Section.AddMenuEntryWithCommandList(FExEditorToolsCommands::Get().ConfigCommands[i], PluginCommands);
// 			}
// 		}
// 	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("ExtendedTools");
			{
				Section.AddEntry(FToolMenuEntry::InitComboButton(
					"ExEditorTools",
					FUIAction(
						FExecuteAction(),
						FCanExecuteAction(),
						FGetActionCheckState()
					),
					FOnGetContent::CreateRaw(this, &FExEditorToolsModule::FillComboButton, PluginCommands),
					LOCTEXT("ExEditorTools", "ExEditorTools"),
					LOCTEXT("ExEditorTools", "ExEditorTools"),
					FSlateIcon(FExEditorToolsStyle::GetStyleSetName(), "ExEditorTools.OpenPluginWindow")
				));
			}
		}
	}
}

void FExEditorToolsModule::RegisterSettings()
{
	ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if(SettingModule)
	{
		SettingModule->RegisterSettings("Project", "Plugins", "ExEditorTools",
			LOCTEXT("ExEditorToolsSettings", "ExEditorToolsSettings"),
			LOCTEXT("ExEditorToolsSettings", "Configure editor buttons."),
			GetMutableDefault<UExEditorToolsConfig>());
	}
}

void FExEditorToolsModule::UnregisterSettings()
{
	ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingModule)
	{
		SettingModule->UnregisterSettings("Project", "Plugins", "ExEditorTools");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExEditorToolsModule, ExEditorTools)