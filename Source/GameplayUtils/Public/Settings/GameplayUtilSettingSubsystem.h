#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayUtilSettingSubsystem.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGameplayUtilSettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void InitGlobalVariables();

	UPROPERTY(Transient)
	TArray<TSoftObjectPtr<UObject>> PermanentObjects;
};
