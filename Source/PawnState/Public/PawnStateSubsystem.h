#pragma once 

#include "CoreMinimal.h"
#include "PawnStateSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PawnStateSubsystem.generated.h"

UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UPawnStateSubsystem* GetSubsystem(const UObject* WorldContextObject);
	const FPawnState& GetPawnState(const FGameplayTag& PawnState);
	bool CheckAssetValid(const UPawnStateAsset* PawnStateAsset);

	EPawnStateRelation GetRelation(const FGameplayTag& NewPawnStateTag, const FGameplayTag& ExistPawnStateTag);


	void OnCheatCreate(UCheatManager* CheatManager);

private:
	void LoadGlobalPawnStateConfig();
	void LoadPawnStateAssets(TSoftObjectPtr<UPawnStateAssets> PawnStateAssets);

	UPROPERTY(Transient)
	TMap<FGameplayTag, UPawnStateAsset*> GlobalPawnStateConfig;

	FPawnState InvalidPawnState;

	FDelegateHandle CheatCreateHandle;
};