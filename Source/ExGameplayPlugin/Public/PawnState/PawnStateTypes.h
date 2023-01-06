#pragma once
#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "PawnStateTypes.generated.h"

UENUM(BlueprintType)
enum class EPawnStateRelation : uint8
{
	E_COEXIST = 0			UMETA(DisplayName = "CoExist"),  //共存
	E_MUTEX					UMETA(DisplayName = "Mutex"),		//互斥
	E_FORBID_ENTER         UMETA(DisplayName = "ForbidEnter"), //禁止
};


USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FPawnStateCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag ToState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EPawnStateRelation Condition;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FPawnStateMapCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<FGameplayTag, EPawnStateRelation> Data;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FPawnStateConfigItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag FromState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FPawnStateCondition> Conditions;
};

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UPawnState : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		FName PawnStateName;

	//本State持有的tag
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer OwnedTags;

	//本State激活所需要的tag
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer RequiredTags;

	//如果被激活，持有这些tag的State将被限制激活
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer BlockOtherTags;

	//如果存在这些tag， 本State被限制激活
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer ActivateBlockedTags;

	//如果本State被激活，持有这些tag的State将退出
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer CancelOtherTags;

	//如果持有这些tag的State激活， 本State会退出
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer CancelActionTags;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FPawnStateInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UObject* SourceObject;

	UPROPERTY(BlueprintReadOnly)
	TArray<UPawnState*> PawnStates;
};
