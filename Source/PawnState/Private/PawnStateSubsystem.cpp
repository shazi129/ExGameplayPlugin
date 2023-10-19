#include "PawnStateSubsystem.h"
#include "ExMacros.h"
#include "PawnStateModule.h"
#include "PawnStateSettings.h"
#include "PawnStateCheatExtension.h"
#include "ExMacros.h"

UPawnStateSubsystem* UPawnStateSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogPawnState, UPawnStateSubsystem, WorldContextObject)
}

UPawnStateAsset* UPawnStateSubsystem::GetPawnStateAsset(const FGameplayTag& StateTag)
{
	if (GlobalPawnStateConfig.Contains(StateTag))
	{
		return GlobalPawnStateConfig[StateTag];
	}
	return nullptr;
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

void UPawnStateSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (CheatCreateHandle.IsValid())
	{
		UCheatManager::UnregisterFromOnCheatManagerCreated(CheatCreateHandle);
	}
}

bool UPawnStateSubsystem::CanEnterPawnState(const FGameplayTag& NewStateTag, const FGameplayTagContainer& ExistStateTags, FString& ErrorMsg)
{
	auto RelationConfig = StateRelations.Find(NewStateTag);
	if (!RelationConfig)
	{
		return true;
	}

	//校验block
	for (const FGameplayTag& ExistTag : ExistStateTags)
	{
		EPawnStateRelation* Relation = RelationConfig->Find(ExistTag);
		if (Relation && *Relation == EPawnStateRelation::E_Block)
		{
			ErrorMsg = FString::Printf(TEXT("%s was blocked by %s"), *NewStateTag.ToString(), *ExistTag.ToString());
			return false;
		}
	}

	//校验Require
	for (auto& RelactionItem : *RelationConfig)
	{
		if (RelactionItem.Value == EPawnStateRelation::E_Require && RelactionItem.Key.MatchesAny(ExistStateTags))
		{
			ErrorMsg = FString::Printf(TEXT("%s miss require: %s"), *NewStateTag.ToString(), *RelactionItem.Key.ToString());
			return false;
		}
	}

	return true;
}

void UPawnStateSubsystem::CollectMutexState(const FGameplayTag& NewStateTag, const FGameplayTagContainer& ExistStateTags, FGameplayTagContainer& OutMutexState)
{
}

EPawnStateRelation UPawnStateSubsystem::GetRelation(const FGameplayTag& NewPawnStateTag, const FGameplayTag& ExistPawnStateTag)
{
	if (StateRelations.Contains(NewPawnStateTag) && StateRelations[NewPawnStateTag].Contains(ExistPawnStateTag))
	{
		return StateRelations[NewPawnStateTag][ExistPawnStateTag];
	}
	return EPawnStateRelation::E_Coexist;
}

void UPawnStateSubsystem::LoadGlobalPawnStateConfig()
{
	for (auto& AssertsParthPtr : GetDefault<UPawnStateSettings>()->GlobalPawnStateSets)
	{
		LoadPawnStateAssets(AssertsParthPtr);
	}
}

void UPawnStateSubsystem::LoadPawnStateAssets(TSoftObjectPtr<UPawnStateSet> PawnStateSet)
{
	if (PawnStateSet.IsNull())
	{
		return;
	}

	UPawnStateSet* StateSetAssets = PawnStateSet.LoadSynchronous();
	if (!StateSetAssets)
	{
		return;
	}

	for (FPawnStateEntry& PawnStateEntry : StateSetAssets->StateSet)
	{
		if (PawnStateEntry.StateAssetPtr.IsNull())
		{
			continue;
		}

		UPawnStateAsset* StateAsset = PawnStateEntry.StateAssetPtr.LoadSynchronous();
		if (!StateAsset || !StateAsset->StateTag.IsValid())
		{
			continue;
		}

		LoadPawnStateAsset(StateAsset);
	}
}

bool UPawnStateSubsystem::LoadPawnStateAsset(UPawnStateAsset* Asset)
{
	if (!Asset) return false;

	if (GlobalPawnStateConfig.Contains(Asset->StateTag))
	{
		UPawnStateAsset* DuplicateAsset = GlobalPawnStateConfig[Asset->StateTag];
		if (DuplicateAsset != Asset)
		{
			PAWNSTATE_LOG(Error, TEXT("%s error, duplicate state asset[%s] %s --> %s"), *FString(__FUNCTION__), *Asset->StateTag.ToString(), *GetNameSafe(DuplicateAsset), *GetNameSafe(Asset));
			return false;
		}
		return true;
	}

	GlobalPawnStateConfig.Add(Asset->StateTag, Asset);

	//构建关系表
	auto& RelationMap = StateRelations.FindOrAdd(Asset->StateTag);

	//自己被block的情况
	for (auto& ActivateBlockPtr : Asset->ActivateBlockedStates)
	{
		UPawnStateAsset* ActivateBlockAsset = ActivateBlockPtr.LoadSynchronous();
		if (!ActivateBlockAsset || !ActivateBlockAsset->StateTag.IsValid())
		{
			PAWNSTATE_LOG(Error, TEXT("%s error, %s cannot load ActivateBlockAsset %s"), *FString(__FUNCTION__), *GetNameSafe(Asset), *ActivateBlockPtr.ToString());
			continue;
		}

		RelationMap.FindOrAdd(ActivateBlockAsset->StateTag) = EPawnStateRelation::E_Block;
	}

	//block别人的情况
	for (auto& BlockOtherPtr : Asset->BlockOtherStates)
	{
		UPawnStateAsset* BlockOtherAsset = BlockOtherPtr.LoadSynchronous();
		if (!BlockOtherAsset || !BlockOtherAsset->StateTag.IsValid())
		{
			PAWNSTATE_LOG(Error, TEXT("%s error, %s cannot loca BlockOtherState %s"), *FString(__FUNCTION__), *GetNameSafe(Asset), *BlockOtherPtr.ToString());
			continue;
		}

		auto& BlockOtherMap = StateRelations.FindOrAdd(BlockOtherAsset->StateTag);
		BlockOtherMap.FindOrAdd(Asset->StateTag) = EPawnStateRelation::E_Block;
	}

	//自己被mutex的情况
	for (auto& CancelledStatePtr : Asset->CancelledStates)
	{
		UPawnStateAsset* CancelledStateAsset = CancelledStatePtr.LoadSynchronous();
		if (!CancelledStateAsset || !CancelledStateAsset->StateTag.IsValid())
		{
			PAWNSTATE_LOG(Error, TEXT("%s error, %s cannot load CancelledStates %s"), *FString(__FUNCTION__), *GetNameSafe(Asset), *CancelledStatePtr.ToString());
			continue;
		}

		RelationMap.FindOrAdd(CancelledStateAsset->StateTag) = EPawnStateRelation::E_Mutex;
	}

	//mutex别人的情况
	for (auto& CancelOtherPtr : Asset->CancelOtherStates)
	{
		UPawnStateAsset* CancelOtherAsset = CancelOtherPtr.LoadSynchronous();
		if (!CancelOtherAsset || !CancelOtherAsset->StateTag.IsValid())
		{
			PAWNSTATE_LOG(Error, TEXT("%s error, %s cannot load CancelOtherStates %s"), *FString(__FUNCTION__), *GetNameSafe(Asset), *CancelOtherPtr.ToString());
			continue;
		}

		auto& CancelOtherMap = StateRelations.FindOrAdd(CancelOtherAsset->StateTag);
		CancelOtherMap.FindOrAdd(Asset->StateTag) = EPawnStateRelation::E_Mutex;
	}

	//require的情况
	for (auto& RequireStatePtr : Asset->RequiredStates)
	{
		UPawnStateAsset* RequireStateAsset = RequireStatePtr.LoadSynchronous();
		if (!RequireStateAsset || !RequireStateAsset->StateTag.IsValid())
		{
			PAWNSTATE_LOG(Error, TEXT("%s error, %s cannot load RequiredStates %s"), *FString(__FUNCTION__), *GetNameSafe(Asset), *RequireStatePtr.ToString());
			continue;
		}

		RelationMap.FindOrAdd(RequireStateAsset->StateTag) = EPawnStateRelation::E_Require;
	}

	return true;
}

void UPawnStateSubsystem::OnCheatCreate(UCheatManager* CheatManager)
{
	if (CheatManager)
	{
		UPawnStateCheatExtension* CheatObject = NewObject<UPawnStateCheatExtension>(CheatManager, UPawnStateCheatExtension::StaticClass());
		CheatManager->AddCheatManagerExtension(CheatObject);
	}
}


void UPawnStateSubsystem::HandleServerMsg(UPawnStateComponent* Component, const FGameplayTag& MsgTag, FInstancedStruct& MsgBody)
{
}

void UPawnStateSubsystem::HandleClientMsg(UPawnStateComponent* Component, const FGameplayTag& MsgTag, FInstancedStruct& MsgBody)
{
}

