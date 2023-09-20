#pragma once
#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "PawnStateTypes.generated.h"

//PawnState间的关系
UENUM(BlueprintType)
enum class EPawnStateRelation : uint8
{
	//共存
	E_Coexist		UMETA(DisplayName = "Coexist"),

	//禁止
	E_Block			UMETA(DisplayName = "Block"),

	//互斥
	E_Mutex		    UMETA(DisplayName = "Mutex"),
};

//pawnstate配置信息
USTRUCT(BlueprintType)
struct PAWNSTATE_API FPawnState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		FString Desc;

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

	FPawnState() {}
	FPawnState(const FGameplayTag& InPawnStateTag)
		: PawnStateTag(InPawnStateTag)
	{

	}

	bool IsValid() const
	{
		return PawnStateTag.IsValid();
	}

	bool operator==(const FPawnState& Other) const
	{
		return PawnStateTag == Other.PawnStateTag;
	}
};

//一个PawnState的配置
UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		FPawnState PawnState;

	UFUNCTION(BlueprintCallable)
		FString ToString()
		{
			return PawnState.PawnStateTag.ToString();
		}
};

//PawnState配置集合
UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateAssets : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<UPawnStateAsset>> PawnStates;
};

//PawnState对应的Instance
USTRUCT(BlueprintType)
struct PAWNSTATE_API FPawnStateInstance
{
	GENERATED_BODY()

	//对应的PawnState
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag PawnStateTag;

	//PawnState持有者，炸弹(SouceObject)会使人受伤害(PawnState)
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> SourceObject = nullptr;

	//PawnState发出者，玩家(Instigator)扔炸弹使人受伤害(PawnState)
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> Instigator = nullptr;

	//对应的PawnStateAsset
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPawnStateAsset> PawnStateAsset;

	//实例ID
	UPROPERTY(BlueprintReadOnly)
	int32 InstanceID = -1;

	FPawnStateInstance(){}

	bool operator==(const FPawnStateInstance& Other) const
	{
		return InstanceID == Other.InstanceID;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("[%s|%s|%s|%s]"), *PawnStateTag.ToString(), *GetNameSafe(PawnStateAsset), *GetNameSafe(SourceObject), *GetNameSafe(Instigator));
	}

	bool IsValid() const
	{
		return PawnStateTag.IsValid() || InstanceID <= 0;
	}
};
