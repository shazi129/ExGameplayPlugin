#include "PawnState/PawnStateCheatExtension.h"
#include "PawnState/PawnStateLibrary.h"
#include "ExGameplayPluginModule.h"

void UPawnStateCheatExtension::DebugPawnState()
{
	if (UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetLocalPawnStateComponent(this))
	{
		const TArray<FPawnStateInstance>& PawnStateInstances = PawnStateComponent->GetPawnStateInstances();
		for (const FPawnStateInstance& Instance : PawnStateInstances)
		{
			EXGAMEPLAY_LOG(Log, TEXT("%s"), *Instance.ToString());
		}
	}
	else
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s, cannot get local pawn state component"), *FString(__FUNCTION__));
	}
}