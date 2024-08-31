#include "Interact/Handlers/InteractItemHandler_ShowWidgetComponent.h"
#include "GameplayUtilsModule.h"

void UInteractItemHandler_ShowWidgetComponent::NativeExecute(const FInteractInstanceData& InstanceData)
{
	bool Visible = false;
	if (InstanceData.InteractState == EInteractState::E_Interactive || InstanceData.InteractState == EInteractState::E_Potential)
	{
		Visible = true;
	}
	if (WidgetComponent.IsValid())
	{
		WidgetComponent->SetVisibility(Visible, true);
	}
}

void UInteractItemHandler_ShowWidgetComponent::NativeOnSourceObjectChange(UObject* OldSourceObject, UObject* NewSourceObject)
{
	AActor* WidgetOwner = GetSourceActor();
	if (WidgetOwner == nullptr)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, cannot get widget owner by %s"), *FString(__FUNCTION__), *GetNameSafe(NewSourceObject));
		return;
	}

	TArray<UActorComponent*> Components;
	WidgetOwner->GetComponents(UWidgetComponent::StaticClass(), Components);
	if (Components.Num() == 0)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, %s has no widget component"), *FString(__FUNCTION__), *GetNameSafe(NewSourceObject));
		return;
	}

	if (WidgetComponentName.IsNone())
	{
		WidgetComponent = Cast<UWidgetComponent>(Components[0]);
	}
	else
	{
		for (auto Component : Components)
		{
			if (Component->GetName().Equals(WidgetComponentName.ToString()))
			{
				WidgetComponent = Cast<UWidgetComponent>(Component);
				break;
			}
		}

		if (!WidgetComponent.IsValid())
		{
			GAMEPLAYUTILS_LOG(Error, TEXT("%s error, %s has no widget component with name:%s"), *FString(__FUNCTION__), *GetNameSafe(NewSourceObject), *WidgetComponentName.ToString());
		}
	}

	if (WidgetComponent.IsValid())
	{
		WidgetComponent->SetVisibility(false, true);
	}
}
