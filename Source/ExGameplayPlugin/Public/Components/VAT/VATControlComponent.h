#pragma once

#include "CoreMinimal.h"
#include "AnimToTextureDataAsset.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Components/VAT/VATDefaultAnimationTypes.h"
#include "Material/MaterialParamter.h"
#include "VATControlComponent.generated.h"

//VAT材质用到的一些参数
UENUM(BlueprintType)
enum class EVATMaterialParamterType: uint8
{
	E_InvalidParamter = 0		UMETA(DisplayName = "InvalidParamter"),
	E_Looping					UMETA(DisplayName = "Looping"),
	E_FrameOffset				UMETA(DisplayName = "FrameOffset"),
	E_NumFrames				UMETA(DisplayName = "NumFrames"),
	E_PlayRate					UMETA(DisplayName = "PlayRate"),
	E_RowsPerFrame				UMETA(DisplayName = "RowsPerFrame"),
	E_SampleRate				UMETA(DisplayName = "SampleRate"),
	E_TimeStartOffset			UMETA(DisplayName = "TimeStartOffset"),
	E_BlendDuration			UMETA(DisplayName = "BlendDuration"),
	E_BlendFrameTime			UMETA(DisplayName = "BlendFrameTime"),
	E_PreFrameOffset			UMETA(DisplayName = "PreFrameOffset"),
	E_PreNumFrames				UMETA(DisplayName = "PreNumFrames"),
};

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UVATMaterialParamterAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TMap<EVATMaterialParamterType, FMaterialParamterConfig> ParamterMap;
};

UCLASS(abstract)
class EXGAMEPLAYPLUGIN_API UVATControlBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	//生成VAT配置，用于解析VAT支持的动画
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VAT")
		UAnimToTextureDataAsset* VATConfigData;

	//起作用的Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VAT")
		FName VATMeshTag;

	//默认动画配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VAT")
		UVATDefaultAnimationAsset* DefaultAnimationConfig;

	//VAT材质参数配置文件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VAT")
		UVATMaterialParamterAsset* DefaultMaterialParamterConfig;

protected:
	//当前VAT支持的动画信息
	TMap<FName, FAnimInfo> AnimInfoMap;

	//配置的动画信息
	TMap<int, FAnimationPlayInfo> AnimationInfoMap;


};


UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UVATControlComponent : public UVATControlBaseComponent
{
	GENERATED_BODY()

public:
	UVATControlComponent();
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void PlayAnimation(const FName& AnimName, float BlendDuration=0.0f);

private:
	//VAT动画涉及的材质
	TArray<UMaterialInstanceDynamic*> MaterialInstances;

	//当前正在播的动画信息
	FAnimInfo* CurrentAnimInfo;

	//VAT材质中用来播放动画的变量
	FMaterialParameterInfo FrameOffsetInfo;
	FMaterialParameterInfo NumFramesInfo;

	//VAT材质中用来Blend的变量
	FMaterialParameterInfo BlendFrameTimeInfo;
	FMaterialParameterInfo BlendDurationInfo;
	FMaterialParameterInfo PreFrameOffsetInfo;
	FMaterialParameterInfo PreNumFramesInfo;
};



