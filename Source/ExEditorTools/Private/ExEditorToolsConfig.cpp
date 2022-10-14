#include "ExEditorToolsConfig.h"

UExEditorToolsContext* UExEditorToolsConfig::GetToolContext()
{
	if (ToolsContext == nullptr && EditorToolsContext.IsValid())
	{
		ToolsContext = Cast<UExEditorToolsContext>(EditorToolsContext.TryLoad());
	}
	return ToolsContext;
}