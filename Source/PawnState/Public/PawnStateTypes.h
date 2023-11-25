#pragma once
#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "InstancedStruct.h"
#include "PawnStateTypes.generated.h"

USTRUCT(BlueprintType)
struct PAWNSTATE_API FRPCParamater
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int ErrCode = 0;

	UPROPERTY(BlueprintReadWrite)
	FString ErrMsg;
};

//PawnState事件触发时机
UENUM(BlueprintType)
enum class EPawnStateEventType : uint8
{
	//在PawnState进入时触发
	E_Enter		UMETA(DisplayName = "Enter"),

	//在PawnState退出时触发
	E_Leave		UMETA(DisplayName = "Leave"),

	//进入退出都会触发
	E_Always	UMETA(DisplayName = "Always"),
};

//PawnState变化时触发的事件
USTRUCT(BlueprintType)
struct PAWNSTATE_API FPawnStateEventItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ShowBasicConfig", EditConditionHides))
	EPawnStateEventType TriggerType = EPawnStateEventType::E_Enter;

	UPROPERTY()
	bool ShowBasicConfig = true;

	virtual ~FPawnStateEventItem(){}
	virtual void Execute(EPawnStateEventType InTriggerType, const FPawnStateInstance& PawnStateInstance, class UPawnStateComponent* PawnStateComponnt) {}
};

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

	//互斥
	E_Require		UMETA(DisplayName = "Require"),
};

//一个PawnState的配置
UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	FString ToString()
	{
		return StateTag.ToString();
	}

	UPROPERTY(EditAnywhere)
		FGameplayTag StateTag;

	//本State激活所需要的前置State
	UPROPERTY(EditAnywhere, meta = (DisplayThumbnail = "false"))
		TArray<TSoftObjectPtr<UPawnStateAsset>> RequiredStates;

	//如果本State被激活，这些State将被限制激活
	UPROPERTY(EditAnywhere, meta = (DisplayThumbnail = "false"))
		TArray<TSoftObjectPtr<UPawnStateAsset>> BlockOtherStates;

	//如果存在这些tag， 本State被限制激活
	UPROPERTY(EditAnywhere, meta = (DisplayThumbnail = "false"))
		TArray<TSoftObjectPtr<UPawnStateAsset>> ActivateBlockedStates;

	//如果本State被激活，持有这些tag的State将退出
	UPROPERTY(EditAnywhere, meta = (DisplayThumbnail = "false"))
		TArray<TSoftObjectPtr<UPawnStateAsset>> CancelOtherStates;

	//如果持有这些tag的State激活， 本State会退出
	UPROPERTY(EditAnywhere, meta = (DisplayThumbnail = "false"))
		TArray<TSoftObjectPtr<UPawnStateAsset>> CancelledStates;

	//State相关事件
	UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/PawnState.PawnStateEventItem", ExcludeBaseStruct))
		TArray<FInstancedStruct> EventList;
};

USTRUCT(BlueprintType)
struct PAWNSTATE_API FPawnStateEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag StateTag;

	UPROPERTY(EditAnywhere)
	FString Description;

	UPROPERTY(EditAnywhere, meta = (DisplayThumbnail = "false"))
	TSoftObjectPtr<UPawnStateAsset> StateAssetPtr;
};

//PawnState配置集合
UCLASS(BlueprintType)
class PAWNSTATE_API UPawnStateSet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (DisplayThumbnail = "false", TitleProperty = "{Description}:{StateTag} -> {StateAssetPtr}"))
	TArray<FPawnStateEntry> StateSet;
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
	TSoftObjectPtr<UObject> SourceObject = nullptr;

	//PawnState发出者，玩家(Instigator)扔炸弹使人受伤害(PawnState)
	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UObject> Instigator = nullptr;

	//实例ID
	TArray<int> IDList;

	FPawnStateInstance(){}

	FString ToString() const
	{
		return FString::Printf(TEXT("[%s|%s|%s|%d]"), *PawnStateTag.ToString(), *GetNameSafe(SourceObject.Get()), *GetNameSafe(Instigator.Get()), IDList.Num());
	}

	bool IsValid() const
	{
		return PawnStateTag.IsValid();
	}
};



