#pragma once

/**
 * 本Subsystem用于封装一些系统的Delegate给蓝图使用 
 */

#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalEventSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FApplicationReactiveDelegate);

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGlobalEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UGlobalEventSubsystem* GetSubsystem(const UObject* WorldContextObject);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	//app切回来时的回调
	UPROPERTY(BlueprintAssignable)
	FApplicationReactiveDelegate ApplicationReactiveDelegate;

private:
	UFUNCTION()
	void OnApplicationReactivated();

	FDelegateHandle ApplicationReactivatedHandle;
};