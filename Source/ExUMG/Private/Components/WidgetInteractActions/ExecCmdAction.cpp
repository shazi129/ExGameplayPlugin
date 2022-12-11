#include "Components/WidgetInteractActions/ExecCmdAction.h"
#include "ExGameplayLibrary.h"

void UExecCmdAction::NativeExecute()
{
	if (!Command.IsEmpty())
	{
		UExGameplayLibrary::ExecCommand(Command);
	}
}
