#pragma once

#include "Interact/InteractTypes.h"
#include "InteractItemInterface.generated.h"

UINTERFACE(BlueprintType)
class GAMEPLAYUTILS_API UInteractItemInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEPLAYUTILS_API IInteractItemInterface
{
	GENERATED_BODY()

public:
	//交互信息有更改时的通知
	UFUNCTION(BlueprintNativeEvent)
	void OnInteractStateChange(const FInteractInstanceData& InteractData);

	//是否可交互
	UFUNCTION(BlueprintNativeEvent)
	bool CanInteract(const FInteractInstanceData& InteractData);

	//交互开始
	UFUNCTION(BlueprintNativeEvent)
	void StartInteract(const FInteractInstanceData& InteractData);

	//交互结束
	UFUNCTION(BlueprintNativeEvent)
	void EndInteract(const FInteractInstanceData& InteractData);

	//获取交互配置
	UFUNCTION(BlueprintNativeEvent)
	TArray<UInteractConfigAsset*> GetConfigAssets();

	//某个配置上有几个角色在交互
	UFUNCTION(BlueprintNativeEvent)
	int32 GetInteractingNum(const FName& ConfigName);

	//某个配置上有几个角色在交互
	UFUNCTION(BlueprintNativeEvent)
	void AddInteractingPawn(const FName& ConfigName, APawn* Pawn);

	//某个配置上有几个角色在交互
	UFUNCTION(BlueprintNativeEvent)
	void RemoveInteractingPawn(const FName& ConfigName, APawn* Pawn);
}; 