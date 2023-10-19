#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "InstancedStruct.h"
#include "GameplayTypes.generated.h"

//在某个端执行行为
UENUM(BlueprintType)
enum class EExecNetMode : uint8
{
	//在客户端触发, 包括Client， ListenServer, Standalone
	E_Client		UMETA(DisplayName = "Client"),

	//在服务端触发， 包括DedicatedServer， ListenServer, Standalone
	E_Server		UMETA(DisplayName = "Server"),

	//不限制
	E_Aways				UMETA(DisplayName = "Aways"),
};

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FFilterActorCondition
{
	GENERATED_BODY()

	//Actor中必须有这些Component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UActorComponent>> RequireComponentClasses;

	//Actor中不能有这些Component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UActorComponent>> ExcludeComponentClasses;

	//Actor必须属于在指定的Class
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<AActor>> ActorClasses;

	//忽略的Actor
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> IgnoreActors;

	bool FilterActorClasses(AActor* Actor) const;
	bool FilterExcludeComponentClasses(AActor* Actor) const;
	bool FilterRequireComponentClasses(AActor* Actor) const;
	bool FilterIgnoreActors(AActor* Actor) const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStringParamDelegate, const FString&, StringParam);

//通用消息体
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FGameplayMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FGameplayTag MsgTag;

	UPROPERTY(BlueprintReadWrite)
		FInstancedStruct MsgBody;

	FGameplayMessage() {}
	FGameplayMessage(const FGameplayTag& InMsgTag)
		: MsgTag(InMsgTag)
	{
	}

	FString ToString() const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayMessageReceivedDelegate, const FGameplayMessage&, Message);