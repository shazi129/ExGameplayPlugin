#pragma once 

/**
 *  用于消息通知，监听的Subsystem
 * 
 */

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTags.h"
#include "InstancedStruct.h"
#include "MessageCenterSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMessageListenDelegate, const FGameplayTag&, MessageTag, const FInstancedStruct&, MessageData);

//初始化Attribute的方式
UENUM(BlueprintType)
enum class EMessageDirection : uint8
{
	//没有方向，都可以监听
	E_None			UMETA(DisplayName = "None"),

	//发送到Client
	E_ToClient			UMETA(DisplayName = "ToClient"),

	//发送到Server
	E_ToServer			UMETA(DisplayName = "ToServer"),
};

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UMessageCenterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UMessageCenterSubsystem* GetSubsystem(const UObject* WorldContextObject);

	virtual void SendMessage(const FGameplayTag& MessageTag, const FInstancedStruct& MessageBody, EMessageDirection Direction=EMessageDirection::E_None);

	virtual int32 RegistListener(const FGameplayTag& MessageTag, UObject* Object, FName FunctionName, bool MatchExactly=true);
	virtual void UnregistListener();
private:
	TMap<FGameplayTag, FMessageListenDelegate> ListenDelegates;
};