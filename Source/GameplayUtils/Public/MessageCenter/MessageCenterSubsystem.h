#pragma once 

/**
 *  用于游戏消息的发送和监听
 * 
 * ********************基本使用方式*******************
 * 
 * 1. 定义消息处理逻辑
 *  void CustomClass::OnMessageReceive(const FGameplayMessage& Message){....}
 * 
 * 2. 注册消息监听
 * UMessageCenterSubsystem* Subsystem = UMessageCenterSubsystem::GetSubsystem(this);
 * Subsystem->GetMessageReceivedDelegateInfo(MessageTag).AddDynamic(this, &CustomClass::OnMessageReceive);
 * 
 * 3. 定义并发送消息
 * FGameplayMessage CustomMessage;
 * CustomMessage.MsgTag = MessageTag;
 * Subsystem->SendMessage(CustomMessage);
 */

#include "Subsystems/GameInstanceSubsystem.h"
#include "RPCFunctionProvider.h"
#include "MessageCenter/GameplayMessages.h"
#include "MessageCenterSubsystem.generated.h"

 //消息方向
UENUM(BlueprintType)
enum class EMessageDirection : uint8
{
	//本地消息
	E_None			UMETA(DisplayName = "None"),

	//需要在服务端处理
	E_ToServer		UMETA(DisplayName = "ToServer"),

	//需要在客户端处理
	E_ToClient		UMETA(DisplayName = "Always"),
};


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
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UMessageCenterSubsystem* GetSubsystem(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	//发送消息
	UFUNCTION(BlueprintCallable)
	void SendMessage(const FGameplayMessage& Message, EMessageDirection Direction= EMessageDirection::E_None);

	UFUNCTION(BlueprintCallable)
	void SendBooleanMessage(const FGameplayTag& MsgTag, bool Value, UObject* SourceObject=nullptr, EMessageDirection Direction = EMessageDirection::E_None);

	//注册接收消息的回调
	UFUNCTION(BlueprintCallable)
	FMessageReceivedDelegateInfo& GetMessageReceivedDelegateInfo(FGameplayTag MsgTag);

private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, FMessageReceivedDelegateInfo> OnMessageReceivedDelegateMap;

public:
	UFUNCTION(BlueprintCallable)
		void RegisterRPCFunctionProvider(TScriptInterface<IRPCFunctionProvider> Provider);

	UFUNCTION(BlueprintCallable)
		void UnregisterRPCFunctionProvider(TScriptInterface<IRPCFunctionProvider> Provider);
private:
	IRPCFunctionProvider* RPCFunctionProvider;


};