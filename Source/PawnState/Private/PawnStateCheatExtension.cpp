#include "PawnStateCheatExtension.h"
#include "PawnStateLibrary.h"
#include "PawnStateModule.h"

void UPawnStateCheatExtension::DebugPawnState()
{
	if (UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetLocalPawnStateComponent(this))
	{
		const TArray<FPawnStateInstance>& PawnStateInstances = PawnStateComponent->GetPawnStateInstances();
		for (const FPawnStateInstance& Instance : PawnStateInstances)
		{
			PAWNSTATE_LOG(Log, TEXT("%s"), *Instance.ToString());
		}
	}
	else
	{
		PAWNSTATE_LOG(Error, TEXT("%s, cannot get local pawn state component"), *FString(__FUNCTION__));
	}
}