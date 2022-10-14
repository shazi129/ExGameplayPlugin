#include "ExEditorToolsCommands.h"
#include "ExEditorToolsConfig.h"
#include "ExEditorToolsModule.h"

#define LOCTEXT_NAMESPACE "FExEditorToolsModule"

void FExEditorToolsCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ExEditorTools", "Bring up ExEditorTools window", EUserInterfaceActionType::Button, FInputGesture());
	RegisterConfigCommands();
}

void FExEditorToolsCommands::RegisterConfigCommands()
{
	UExEditorToolsContext* Context = GetMutableDefault<UExEditorToolsConfig>()->GetToolContext();
	if (Context)
	{
		for (const auto& ToolItem : Context->ToolItems)
		{
			TSharedPtr<FUICommandInfo>& Command = ConfigCommands.Add_GetRef(TSharedPtr<FUICommandInfo>());
			MakeUICommand_InternalUseOnly(this, Command, TEXT(LOCTEXT_NAMESPACE), *ToolItem.CommandName, *(ToolItem.CommandName + "_ToolTip"),
				TCHAR_TO_ANSI(*("." + ToolItem.CommandName)), *ToolItem.FriendlyName, *ToolItem.Description, EUserInterfaceActionType::Button, FInputGesture());
		}
	}
	else
	{
		UE_LOG(LogExEditorTools, Error, TEXT("FExEditorToolsCommands::RegisterConfigCommands error, Cannot find Editor tool Context"));
	}
}

#undef LOCTEXT_NAMESPACE