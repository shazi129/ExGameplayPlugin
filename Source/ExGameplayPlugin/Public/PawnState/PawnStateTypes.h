#pragma once
#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "PawnStateTypes.generated.h"

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UPawnState : public UDataAsset
{
	GENERATED_BODY()

public:

	//本State持有的tag
	UPROPERTY(EditAnywhere)
		FGameplayTag PawnStateTag;

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
		FGameplayTagContainer CancelledTags;
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FPawnStateInstance
{
	GENERATED_BODY()

	//PawnState持有者，炸弹(SouceObject)会使人受伤害(PawnState)
	UPROPERTY(BlueprintReadOnly)
	UObject* SourceObject;

	//PawnState出发这，玩家(Instigator)扔炸弹使人受伤害(PawnState)
	UPROPERTY(BlueprintReadOnly)
	UObject* Instigator;

	UPROPERTY(BlueprintReadOnly)
	UPawnState* PawnState;

	FPawnStateInstance()
	{
		PawnState = nullptr;
		SourceObject = nullptr;
		Instigator = nullptr;
	}

	FPawnStateInstance(const FPawnStateInstance& Other)
	{
		PawnState = Other.PawnState;
		SourceObject = Other.SourceObject;
		Instigator = Other.Instigator;
	}

	FPawnStateInstance(UPawnState* InPawnState, UObject* InSourceObject, UObject* InInstigator=nullptr)
	{
		PawnState = InPawnState;
		SourceObject = InSourceObject;
		Instigator = InInstigator;
	}

	bool operator==(const FPawnStateInstance& Other) const
	{
		return PawnState == Other.PawnState
			&& SourceObject == Other.SourceObject
			&& Instigator == Other.Instigator;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("[%s|%s|%s]"), *PawnState->PawnStateTag.ToString(), *GetNameSafe(SourceObject), *GetNameSafe(Instigator));
	}

	bool IsValid() const
	{
		return PawnState != nullptr && SourceObject != nullptr;
	}
};
