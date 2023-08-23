#pragma once 

#include "CoreMinimal.h"
#include "ExMacros.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ExGameplaySettingSubsystem.generated.h"

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UExGameplaySettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_GET_GAMEINSTANCE_SUBSYSTEM(UExGameplaySettingSubsystem, LogTemp)

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void OnCheatManagerCreate(UCheatManager* CheatManager);
	void InitGlobalVariables();

private:
	FDelegateHandle CheatManagerCreateHandle;
};
