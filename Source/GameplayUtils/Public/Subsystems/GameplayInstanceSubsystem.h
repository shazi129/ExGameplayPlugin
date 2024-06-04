#pragma once

/**
 * 游戏中一些通用的GameInstanceSystem功能, 例如保存一些值
 */

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayInstanceSubsystem.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGameplayInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UGameplayInstanceSubsystem* GetSubsystem(const UObject* WorldContextObject);
};
