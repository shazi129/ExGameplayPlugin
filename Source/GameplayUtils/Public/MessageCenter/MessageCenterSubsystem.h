#pragma once 

/**
 *  用于消息通知，监听的Subsystem
 * 
 */

#include "Subsystems/GameInstanceSubsystem.h"
#include "RPCFunctionProvider.h"
#include "MessageCenterSubsystem.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FMessageReceivedDelegateInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FGameplayMessageReceivedDelegate Delegate;
};

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UMessageCenterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	static UMessageCenterSubsystem* GetSubsystem(const UObject* WorldContextObject);

public:
	UFUNCTION(BlueprintCallable)
	void SendMsgToServer(const FGameplayMessage& Message);

	UFUNCTION(BlueprintCallable)
	void SendMsgToClient(const FGameplayMessage& Message);


	UFUNCTION(BlueprintCallable)
		void RegisterRPCFunctionProvider(TScriptInterface<IRPCFunctionProvider> Provider);

	UFUNCTION(BlueprintCallable)
		void UnregisterRPCFunctionProvider(TScriptInterface<IRPCFunctionProvider> Provider);
private:
	IRPCFunctionProvider* RPCFunctionProvider;

public:
	UFUNCTION(BlueprintCallable)
	FMessageReceivedDelegateInfo& GetMessageReceivedDelegateInfo(FGameplayTag MsgTag);

	void OnMessageReceived(const FGameplayMessage& Message);


private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, FMessageReceivedDelegateInfo> OnMessageReceivedDelegateMap;
};