#include "InputActionHandler.h"
#include "ExInputSystemModule.h"

UWorld* UInputActionHandler::GetWorld() const
{
	if (SourceObject != nullptr)
	{
		UWorld* World = SourceObject->GetWorld();
		if (World == nullptr)
		{
			EXINPUTSYSTEM_LOG(Warning, TEXT("UInputBindingActionHandler::GetWorld error, Cannot Get World from object[%s]"), *SourceObject->GetName());
		}
		return World;
	}

	EXINPUTSYSTEM_LOG(Warning, TEXT("UInputBindingActionHandler::GetWorld error, SourceObject is null: [%s]"), *GetName());
	return nullptr;
}

void UInputActionHandler::SetSourceObject(UObject* Object)
{
	SourceObject = Object;
}