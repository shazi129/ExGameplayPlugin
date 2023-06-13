#include "ExInputTypes.h"
#include "ExInputSubsystem.h"

UInputActionHandler* FInputBindingConfig::GetInputHandler(UWorld* ContextWorld)
{
	if (HandlerInstance == nullptr)
	{
		if (BindingType == EInputBindingType::E_Instanced)
		{
			HandlerInstance = InputHandler;
		}
		else
		{
			TSubclassOf<UInputActionHandler> HandlerClass = InputHandlerClass.LoadSynchronous();
			if (HandlerClass)
			{
				HandlerInstance = UExInputSubsystem::GetInputSubsystem(ContextWorld)->CreateInputActionHandler(HandlerClass);
			}
		}
	}

	return HandlerInstance;
}