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