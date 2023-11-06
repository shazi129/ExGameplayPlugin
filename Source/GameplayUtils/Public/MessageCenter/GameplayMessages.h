#pragma once

#include "GameplayTags.h"
#include "InstancedStruct.h"
#include "GameplayMessages.generated.h"

//通用消息体
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FGameplayMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	UObject* SourceObject;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag MsgTag;

	UPROPERTY(BlueprintReadWrite)
	FInstancedStruct MsgBody;

	FGameplayMessage() {}
	FGameplayMessage(const FGameplayTag& InMsgTag, UObject* InSourceObject=nullptr)
		: SourceObject(InSourceObject)
		, MsgTag(InMsgTag)
	{
	}

	FString ToString() const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayMessageReceivedDelegate, const FGameplayMessage&, Message);


USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FBooleanMessagBody
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool Value;

	FBooleanMessagBody(){}
	FBooleanMessagBody(bool InValue)
	: Value(InValue){}
};
