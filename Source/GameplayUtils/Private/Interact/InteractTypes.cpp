#include "Interact/InteractTypes.h"

FInteractInstanceData::FInteractInstanceData(TScriptInterface<IInteractItemInterface> InItemInterface, AActor* InItemActor, const FInteractConfigData& InConfigData)
{
	ItemInterface = InItemInterface;
	ItemActor = InItemActor;
	InteractState = EInteractState::E_None;
	SubPotentialIndex = -1;
	InteractOrder = -1;
	NeedNofity = false;
	Enable = InConfigData.Enable;
	Instigator = nullptr;
	ConfigData = InConfigData;
}

bool FInteractInstanceData::IsValid() const
{
	return ItemInterface.GetObject() && ItemActor.IsValid();
}

void FInteractRange::Reset()
{
	Direction.Set(0, 0, 0);
	Angle = 0;
	IgnoreAxis = EInteractAngleAxis::E_NONE;
}


