#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayCheatSubsystem.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGameplayCheatSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void OnCheatManagerCreate(UCheatManager* CheatManager);

private:
	
	FDelegateHandle CheatCreateHandle;
};
