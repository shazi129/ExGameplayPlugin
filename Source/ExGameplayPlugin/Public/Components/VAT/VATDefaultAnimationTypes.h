#pragma once

#include "CoreMinimal.h"
#include "VATDefaultAnimationTypes.generated.h"

//VAT默认动画类型
UENUM(BlueprintType)
enum EVATAnimationType
{
	E_NONE = 0			UMETA(DisplayName = "None"),
	E_Idle				UMETA(DisplayName = "Idle"),
	E_WalkInPace		UMETA(DisplayName = "Walk In Pace"),
	E_WalkFwd			UMETA(DisplayName = "Walk Forward"),
	E_RunFwd			UMETA(DisplayName = "Run Forward"),
	E_Jump				UMETA(DisplayName = "Jump"),
	E_FallLoop			UMETA(DisplayName = "Fall Loop"),
	E_Land				UMETA(DisplayName = "Land"),

	E_Default_MAX		UMETA(DisplayName = "Default Max")
};

//默认动画配置
USTRUCT(BlueprintType)
struct FAnimationPlayInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName AnimationName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BlendTime;
};

USTRUCT(BlueprintType)
struct FExAnimationInfo
{
	GENERATED_BODY()
	FAnimationPlayInfo PlayInfo;
	FAnimInfo AnimInfo;
};


UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UVATDefaultAnimationAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TMap<TEnumAsByte<EVATAnimationType>, FAnimationPlayInfo> DefaultAnimationInfo;

	FAnimationPlayInfo* GetAnimationInfo(int AnimationType)
	{
		if (AnimationType > EVATAnimationType::E_NONE && AnimationType < EVATAnimationType::E_Default_MAX)
		{
			return DefaultAnimationInfo.Find((EVATAnimationType)AnimationType);
		}
		return nullptr;
	}
};


