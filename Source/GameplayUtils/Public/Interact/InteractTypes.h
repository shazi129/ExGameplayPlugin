#pragma once

#include "GameplayTags.h"
#include "InteractTypes.generated.h"

//忽略的轴
UENUM(BlueprintType)
enum class EInteractAngleAxis : uint8
{
	E_NONE      UMETA(DisplayName = "None"),  //没有忽略的轴
	E_X			 UMETA(DisplayName = "X"),
	E_Y			 UMETA(DisplayName = "Y"),
	E_Z			 UMETA(DisplayName = "Z"),
};

//在那个端生效, 因为都是在Client上响应输入，但有些行为需要Server确认
UENUM(BlueprintType)
enum class EInteractRole : uint8
{
	E_Client         UMETA(DisplayName = "Client"),  //只在客户端产生交互
	E_Server           UMETA(DisplayName = "Server"), //需要服务端确认
};

//交互行为类型
UENUM(BlueprintType)
enum class EInteractState : uint8
{
	E_None = 0				 UMETA(DisplayName = "None"),  //没有交互
	E_Potential            UMETA(DisplayName = "Potential"), //还没到交互，但有提示
	E_Interactive          UMETA(DisplayName = "Interactive"), //可交互的
	E_Interacting          UMETA(DisplayName = "Interactiving"), //正在交互
};

//交互的范围描述
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FInteractRange
{
	GENERATED_BODY()

	//方向
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Direction;

	//范围角度
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Angle;

	//忽略的轴, 例如忽略z轴，夹角就按xy平面来算
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EInteractAngleAxis> IgnoreAxis;

	void Reset();
};

//配置数据，用于在蓝图中配置，一般运行时不做修改
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FInteractConfigData
{
	GENERATED_BODY()

	//此配置名字，用于标识这个配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ConfigName;

	//可以触发交互的输入Tag
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag;

	//交互优先级
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Priority = 0;

	//可交互数量，例如一张椅子可供三个人坐
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxInteractNum = 1;

	//交互范围的中心点偏移，相对于Component的世界坐标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CenterOffset;

	//提示区域半径，例如300展示蓝色图标，200展示红色图标, 从大到小
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> PotentialRadiusList;

	//交互区域半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractiveRadius;

	//指定范围
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInteractRange> Ranges;

	//指定交互端
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EInteractRole> InteractRole;

	//只在最高优先级时才相应, 防止出现多个相同的输入时相应多个事件
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool OnlyInHighest = true;

	//是否启用了交互
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Enable = true;
};

//
UCLASS(BlueprintType, Blueprintable, CollapseCategories)
class GAMEPLAYUTILS_API UInteractConfigAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FInteractConfigData ConfigData;
};

//运行时的交互数据，用于各种操作或是通知
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FInteractInstanceData
{
	GENERATED_BODY()

	FInteractInstanceData() = default;

	FInteractInstanceData(TScriptInterface<class IInteractItemInterface> InItemInterface, AActor* InItemActor, const FInteractConfigData& InConfigData);

	//交互物体接口
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<class IInteractItemInterface> ItemInterface;

	//交互物体Actor
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> ItemActor;

	//谁触发的这个交互
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<APawn> Instigator;

	//本条交互数据对应的交互类型，结合交互组件可以找到相关的配置数据
	UPROPERTY(BlueprintReadOnly)
	FInteractConfigData ConfigData;

	//本组件的交互状态
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EInteractState> InteractState;

	//本组件交互提示，区分远近距离
	UPROPERTY(BlueprintReadOnly)
	int32 SubPotentialIndex;

	//本组件的响应次序
	UPROPERTY(BlueprintReadOnly)
	int InteractOrder;

	//跟交互者的距离
	UPROPERTY(BlueprintReadOnly)
	int Distance;

	//跟交互者的夹角
	UPROPERTY(BlueprintReadOnly)
	float Angle;

	//是否需要通知
	bool NeedNofity;

	//是否运行开启
	bool Enable;

	bool IsValid() const;
};

//某个Object对应的交互信息
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FObjectInstanceDataInfo
{
	GENERATED_BODY()

	//Object是否可移动
	bool Moveable = false;

	//每个Tick都需要计算的数据
	TArray<FInteractInstanceData> InstanceDataList;
};

//网络复制用的结构，FInteractInstanceData的最小集
USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FInteractReplicateData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName ConfigName;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<APawn> Instigator;
};

//各种Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractStateChange, const FInteractInstanceData&, InteractData);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FCanInteract, const FInteractInstanceData&, InteractData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, const FInteractInstanceData&, InteractData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompInteractDelegate);
