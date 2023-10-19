#pragma once 

#include "CoreMinimal.h"
#include "PawnStateSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PawnStateComponent.h"
#include "PawnStateSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNotifyMsgDelegate, const FGameplayTag&, MsgTag, const FInstancedStruct&, MsgBody);

UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UPawnStateSubsystem* GetSubsystem(const UObject* WorldContextObject);

	UPawnStateAsset* GetPawnStateAsset(const FGameplayTag& StateTag);

	/**
	 * @brief 判断能否进入一个State
	 * @param NewStateTag  要进入的State
	 * @param ExistStateTags  当前已存在的State
	 * @param ErrorMsg 不能进入的提示
	 * @return  是否能进入
	*/
	bool CanEnterPawnState(const FGameplayTag& NewStateTag, const FGameplayTagContainer& ExistStateTags, FString& ErrorMsg);

	/**
	 * @brief 从现有的State中收集
	 * @param NewStateTag 
	 * @param ExistStateTags 
	 * @param OutMutexState 
	*/
	void CollectMutexState(const FGameplayTag& NewStateTag, const FGameplayTagContainer& ExistStateTags, FGameplayTagContainer& OutMutexState);

	EPawnStateRelation GetRelation(const FGameplayTag& NewPawnStateTag, const FGameplayTag& ExistPawnStateTag);

	const TMap<FGameplayTag, TMap<FGameplayTag, EPawnStateRelation>>& GetRelationConfig(){return StateRelations;}

	bool LoadPawnStateAsset(UPawnStateAsset* Asset);

private:
	void LoadGlobalPawnStateConfig();
	void LoadPawnStateAssets(TSoftObjectPtr<UPawnStateSet> PawnStateSet);
	

	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateAsset*> GlobalPawnStateConfig;

	TMap<FGameplayTag, TMap<FGameplayTag, EPawnStateRelation>> StateRelations;

#pragma region /////////////////////////////////////Cheate相关////////////////
public:
	void OnCheatCreate(UCheatManager* CheatManager);

	void HandleServerMsg(UPawnStateComponent* Component, const FGameplayTag& MsgTag, FInstancedStruct& MsgBody);
	void HandleClientMsg(UPawnStateComponent* Component, const FGameplayTag& MsgTag, FInstancedStruct& MsgBody);

private:
	FDelegateHandle CheatCreateHandle;
	//TSoftObjectPtr<UPawnStateCheatExtension> CheatObject;
#pragma endregion
};