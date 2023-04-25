#include "ExInputTypes.h"

UInputActionHandler* FInputBindingConfig::GetInputHandler()
{
	if (!CurrentHandler)
	{
		if (BindingType == EInputBindingType::E_Instanced)
		{
			CurrentHandler = InputHandler;
		}
		else
		{
			CurrentHandler = SoftInputHandler.LoadSynchronous();
		}
	}
	return CurrentHandler;
}