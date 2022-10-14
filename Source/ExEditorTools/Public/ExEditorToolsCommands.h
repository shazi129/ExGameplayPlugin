#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ExEditorToolsStyle.h"

class FExEditorToolsCommands : public TCommands<FExEditorToolsCommands>
{
public:

	FExEditorToolsCommands()
		: TCommands<FExEditorToolsCommands>(TEXT("ExEditorTools"), NSLOCTEXT("Contexts", "ExEditorTools", "ExEditorTools Plugin"), NAME_None, FExEditorToolsStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	void RegisterConfigCommands();

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
	TArray<TSharedPtr< FUICommandInfo >> ConfigCommands;
};
