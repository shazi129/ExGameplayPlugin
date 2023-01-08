#include "Cheats/ExCheatManagerExtension.h"
#include "ExGameplayPluginModule.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"

void UExCheatManagerExtension::PrintStreamingLevelInfo()
{
	const TIndirectArray<FWorldContext>& WorldContextList = GEngine->GetWorldContexts();
	for (const FWorldContext& WorldContext : WorldContextList)
	{
		UWorld* World = WorldContext.World();
		if (World && World->IsGameWorld() && World->WorldComposition)
		{
			EXGAMEPLAY_LOG(Log, TEXT("=================%s Name[%s] NetMode%d]================="), *FString(__FUNCTION__), *World->GetName(), World->GetNetMode());
			for (TObjectPtr<ULevelStreaming>& LevelStreaming : World->WorldComposition->TilesStreaming)
			{
				FString LevelName = FPackageName::GetShortName(LevelStreaming->PackageNameToLoad);
				EXGAMEPLAY_LOG(Log, TEXT("%s --> Priority[%d], State[%s], BlockOnLoad[%d]"),
										*LevelName, LevelStreaming->GetPriority(), ULevelStreaming::EnumToString(LevelStreaming->GetCurrentState()), LevelStreaming->bShouldBlockOnLoad);
			}
			EXGAMEPLAY_LOG(Log, TEXT("==============%s[%s] end=============="), *FString(__FUNCTION__), *World->GetName());
		}
	}
}

void UExCheatManagerExtension::ShowGlobals()
{
	EXGAMEPLAY_LOG(Log, TEXT("=================%s================="), *FString(__FUNCTION__));

	//FAsyncLoadingThreadSettings::Get();
	EXGAMEPLAY_LOG(Log, TEXT("FAsyncLoadingThreadSettings: bAsyncLoadingThreadEnabled[%d], bAsyncPostLoadEnabled[%d]"), *FString(__FUNCTION__));

	EXGAMEPLAY_LOG(Log, TEXT("=================%s End============="), *FString(__FUNCTION__));
}