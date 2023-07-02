#include "GameFramework/ExCharacterMovementComponent.h"

UExCharacterMovementComponent::UExCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bMovementSyncEnable(true)
{

}

void UExCharacterMovementComponent::SetMovementSyncEnable(bool Value)
{
	bMovementSyncEnable = Value;
}

void UExCharacterMovementComponent::CallServerMovePacked(const FSavedMove_Character* NewMove, const FSavedMove_Character* PendingMove, const FSavedMove_Character* OldMove)
{
	if (bMovementSyncEnable)
	{
		Super::CallServerMovePacked(NewMove, PendingMove, OldMove);
	}
	else
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (!ClientData)
		{
			return;
		}

		for (int i = ClientData->SavedMoves.Num() - 1; i >= 0; i--)
		{
			if (ClientData->SavedMoves[i].Get() != NewMove)
			{
				ClientData->FreeMove(ClientData->SavedMoves[i]);
				ClientData->SavedMoves.RemoveAt(i);
			}
		}
	}
}
