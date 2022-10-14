// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "InteractItemComponent.generated.h"

//交互行为类型
UENUM(BlueprintType)
enum EInteractState
{
	E_Interact_None = 0				 UMETA(DisplayName = "None"),  //没有交互
	E_Interact_Potential               UMETA(DisplayName = "Potential"), //还没到交互，但有提示
	E_Interact_Interactive             UMETA(DisplayName = "Interactive"), //可交互的
};

//忽略的轴
UENUM(BlueprintType)
enum EInteractAngleAxis
{
	E_Interact_Angle_NONE         UMETA(DisplayName = "None"),  //没有忽略的轴
	E_Interact_Angle_X			 UMETA(DisplayName = "X"),
	E_Interact_Angle_Y			 UMETA(DisplayName = "Y"),
	E_Interact_Angle_Z			 UMETA(DisplayName = "Z"),
};

//在那个端生效
UENUM(BlueprintType)
enum EInteractRole
{
	E_Interact_Role_OnlyClient         UMETA(DisplayName = "Client"),  //只在客户端产生交互
	E_Interact_Role_Both                UMETA(DisplayName = "Both"), //服务端也要交互
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FInteractRange
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
};

//配置数据，用于在蓝图中配置，一般运行时不做修改
USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FInteractConfigData
{
	GENERATED_BODY()

	//交互物体类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InteractType;

	//交互优先级
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Priority;

	//交互触发的Tag
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag InteractTag;

	//交互范围的中心点偏移，相对于Component的世界坐标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector CenterOffset;

	//提示区域半径，不小于交互半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PotentialRadius;

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

class UInteractItemComponent;

//交互数据，用于各种操作
USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FInteractData
{
	GENERATED_BODY()

	FInteractData() = default;

	FInteractData(AActor* Initiator, UInteractItemComponent* InteractComponent, const FGameplayTag& InteractType);

	//交互发起人
	UPROPERTY(BlueprintReadOnly)
		AActor* Initiator;

	//交互的组件
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<UInteractItemComponent> InteractItemComponent;

	//本条交互数据对应的交互类型，结合交互组件可以找到相关的配置数据
	UPROPERTY(BlueprintReadOnly)
		FGameplayTag InteractType;

	//本组件的交互状态
	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EInteractState> InteractState;

	//本组件的响应次序
	UPROPERTY(BlueprintReadOnly)
		int InteractOrder;

	UPROPERTY(BlueprintReadOnly)
		int Distance;

	UPROPERTY(BlueprintReadOnly)
		float Angle;

	UPROPERTY(BlueprintReadOnly)
		bool NeedNofity;

	UPROPERTY(BlueprintReadOnly)
		bool Enable;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractStateChange, const FInteractData&, InteractData);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FCanInteract, const FInteractData&, InteractData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, const FInteractData&, InteractData);


/**
*  挂在可交互物品身上，为玩家提供交互信息
*/

UCLASS(ClassGroup = (Interact), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UInteractItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	//交互信息有更改时的通知
	UPROPERTY(BlueprintAssignable)
		FOnInteractStateChange OnInteractStateChange;

	UPROPERTY(BlueprintReadWrite)
		FCanInteract CanInteractDelegate;

	//响应
	UPROPERTY(BlueprintAssignable)
		FOnInteract OnInteractDelegate;

public:
	// Sets default values for this component's properties
	UInteractItemComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		class UInteractManagerComponent* GetInteractManagerComponent();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Config")
		TArray<FInteractConfigData> InteractConfigList;

	FInteractConfigData* GetConfigData(const FGameplayTag& InteractType);

	UFUNCTION(BlueprintCallable)
		void NotifyInteractStateChange(const FInteractData& InteractData);
	UFUNCTION(BlueprintNativeEvent, DisplayName = "NotifyInteractStateChange")
		void BP_NotifyInteractStateChange(const FInteractData& InteractData);

	UFUNCTION(BlueprintCallable)
		bool CanInteract(const FInteractData& InteractData);
	UFUNCTION(BlueprintNativeEvent, DisplayName = "CanInteract")
		bool BP_CanInteract(const FInteractData& InteractData);

	

	UFUNCTION(BlueprintCallable)
		bool OnInteract(const FInteractData& InteractData);

	UFUNCTION(BlueprintNativeEvent, DisplayName = "OnInteract")
		void BP_OnInteract(const FInteractData& InteractData);

	UFUNCTION(BlueprintCallable)
		void SetEnable(bool Enable, FGameplayTag InteractType);
	void InternalSetEnable(bool Enable, FGameplayTag InteractType);
	UFUNCTION(Client, Reliable, WithValidation)
		void ClientSetEnable(bool Enable, FGameplayTag InteractType);
};


