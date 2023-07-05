#pragma once 

#include "CoreMinimal.h"
#include "ExGameFeaturesSettings.h"
#include "ExMacros.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ExGameFeaturesSubsystem.generated.h"

USTRUCT()
struct EXGAMEFEATURES_API FModularActionsInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TSoftObjectPtr<UGameFeatureAction>> ModularActrions;

	bool IsActivated = false;

	FString ToString() const;
};

UCLASS(BlueprintType)
class EXGAMEFEATURES_API UExGameFeaturesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_GET_GAMEINSTANCE_SUBSYSTEM(UExGameFeaturesSubsystem, LogTemp);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	inline const TMap<FGameplayTag, FModularActionsInstance>& GetModularActions()
	{
		return ModularActionsMap;
	}

	UFUNCTION(BlueprintCallable)
	bool AddModularActions(const FGameplayTag& ModularTag, const FModularActions& ModularActions);

	UFUNCTION(BlueprintCallable)
	bool RemoveModularActions(const FGameplayTag& ModularTag);

	UFUNCTION(BlueprintCallable)
	bool ActivateModule(const FGameplayTag& ModularTag);

	UFUNCTION(BlueprintCallable)
	bool DeactivateModule(const FGameplayTag& ModularTag);

	UFUNCTION(BlueprintCallable)
	bool LoadModuarActionData(TSoftObjectPtr<UModularActionsAssetData> ModularActionAssetData);

private:
	bool LoadDefaultModularActions();
	void OnCheatCreate(UCheatManager* CheatManager);

private:
	UPROPERTY(Transient)
		TMap<FGameplayTag, FModularActionsInstance> ModularActionsMap;

	UPROPERTY(Transient)
		TMap<FString, TObjectPtr<UModularActionsAssetData>> ModularActionAssetMap;

	FDelegateHandle CheatCreateHandle;
};