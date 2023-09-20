#include "PawnStateSubsystem.h"
#include "ExMacros.h"
#include "PawnStateModule.h"
#include "PawnStateSettings.h"
#include "PawnStateCheatExtension.h"

UPawnStateSubsystem* UPawnStateSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogPawnState, UPawnStateSubsystem, WorldContextObject)
}

void UPawnStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadGlobalPawnStateConfig();

#if UE_WITH_CHEAT_MANAGER
	CheatCreateHandle = UCheatManager::RegisterForOnCheatManagerCreated(
		FOnCheatManagerCreated::FDelegate::CreateUObject(this, &UPawnStateSubsystem::OnCheatCreate));
#endif
}

void UPawnStateSubsystem::OnCheatCreate(UCheatManager* CheatManager)
{
	if (CheatManager)
	{
		CheatManager->AddCheatManagerExtension(NewObject<UPawnStateCheatExtension>(CheatManager, UPawnStateCheatExtension::StaticClass()));
	}
}

void UPawnStateSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (CheatCreateHandle.IsValid())
	{
		UCheatManager::UnregisterFromOnCheatManagerCreated(CheatCreateHandle);
	}
}


const FPawnState& UPawnStateSubsystem::GetPawnState(const FGameplayTag& PawnState)
{
	if (GlobalPawnStateConfig.Contains(PawnState))
	{
		if (GlobalPawnStateConfig[PawnState])
		{
			return GlobalPawnStateConfig[PawnState]->PawnState;
		}
	}
	return InvalidPawnState;
}

bool UPawnStateSubsystem::CheckAssetValid(const UPawnStateAsset* PawnStateAsset)
{
	if (!PawnStateAsset || !PawnStateAsset->PawnState.IsValid())
	{
		return false;
	}
	
	auto GlobalAssetPtr = GlobalPawnStateConfig.Find(PawnStateAsset->PawnState.PawnStateTag);
	if (!GlobalAssetPtr)
	{
		return false;
	}
	return *GlobalAssetPtr == PawnStateAsset;
}

EPawnStateRelation UPawnStateSubsystem::GetRelation(const FGameplayTag& NewPawnStateTag, const FGameplayTag& ExistPawnStateTag)
{
	//合法性check
	const FPawnState& NewPawnState = GetPawnState(NewPawnStateTag);
	const FPawnState& ExistPawnState = GetPawnState(ExistPawnStateTag);
	if (!NewPawnState.IsValid() || !ExistPawnState.IsValid())
	{
		return EPawnStateRelation::E_Block;
	}

	//block检查
	if (NewPawnStateTag.MatchesAny(ExistPawnState.BlockOtherTags)) //别人block我
	{
		return EPawnStateRelation::E_Block;
	}
	else if (ExistPawnState.PawnStateTag.MatchesAny(NewPawnState.ActivateBlockedTags)) //我主动被block
	{
		return EPawnStateRelation::E_Block;
	}

	//mutex检查
	if (NewPawnStateTag.MatchesAny(ExistPawnState.CancelOtherTags)) //别人mutex我
	{
		return EPawnStateRelation::E_Mutex;
	}
	else if (ExistPawnState.PawnStateTag.MatchesAny(NewPawnState.CancelledTags)) //我主动mutex他人
	{
		return EPawnStateRelation::E_Mutex;
	}

	return EPawnStateRelation::E_Coexist;
}


void UPawnStateSubsystem::LoadGlobalPawnStateConfig()
{
	for (auto& AssertsParthPtr : GetDefault<UPawnStateSettings>()->GlobalPawnStateAssets)
	{
		LoadPawnStateAssets(AssertsParthPtr);
	}
}

void UPawnStateSubsystem::LoadPawnStateAssets(TSoftObjectPtr<UPawnStateAssets> PawnStateAssets)
{
	if (PawnStateAssets.IsNull())
	{
		return;
	}

	UPawnStateAssets* Assets = PawnStateAssets.LoadSynchronous();
	if (!Assets)
	{
		return;
	}

	for (auto& PawnStatePathPtr : Assets->PawnStates)
	{
		if (PawnStatePathPtr.IsNull())
		{
			continue;
		}

		UPawnStateAsset* Asset = PawnStatePathPtr.LoadSynchronous();
		if (!Assets || !Asset->PawnState.IsValid())
		{
			continue;
		}

		FGameplayTag& NewTag = Asset->PawnState.PawnStateTag;
		if (GlobalPawnStateConfig.Contains(NewTag))
		{
			PAWNSTATE_LOG(Log, TEXT("%s error pawnstate: %s, %s"), *FString(__FUNCTION__), *GetNameSafe(Asset), *GetNameSafe(GlobalPawnStateConfig[NewTag]));
			continue;
		}

		GlobalPawnStateConfig.Add(NewTag, Asset);
	}
}

