#include "Components/VAT/VATControlComponent.h"
#include "ExGameplayPluginModule.h"
#include "Kismet/KismetSystemLibrary.h"

void UVATControlBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	//初始化动画信息
	if (VATConfigData != nullptr)
	{
		int AnimationNum = FMath::Min(VATConfigData->AnimSequences.Num(), VATConfigData->Animations.Num());
		if (AnimationNum == 0)
		{
			EXGAMEPLAY_LOG(Warning, TEXT("%s: VATConfigData has not configured animation"), *FString(__FUNCTION__));
		}

		for (int i = 0; i < AnimationNum; i++)
		{
			FAnimSequenceInfo& AnimSequenceInfo = VATConfigData->AnimSequences[i];
			FAnimInfo& AnimInfo = VATConfigData->Animations[i];

			if (AnimSequenceInfo.AnimSequence != nullptr)
			{
				AnimInfoMap.Add(AnimSequenceInfo.AnimSequence->GetFName(), AnimInfo);
			}
		}
	}

	//填充配置数据
	if (DefaultAnimationConfig)
	{
		for (auto DefaultAnimationInfoItem : DefaultAnimationConfig->DefaultAnimationInfo)
		{
			AnimationInfoMap.Add(DefaultAnimationInfoItem.Key, DefaultAnimationInfoItem.Value);
		}
	}
}


UVATControlComponent::UVATControlComponent()
{
	CurrentAnimInfo = nullptr;

	FrameOffsetInfo = FMaterialParameterInfo(FName("FrameOffset (S)"), EMaterialParameterAssociation::LayerParameter, 0);
	NumFramesInfo = FMaterialParameterInfo("NumFrames (S)", EMaterialParameterAssociation::LayerParameter, 0);

	BlendFrameTimeInfo = FMaterialParameterInfo("BlendFrameTime", EMaterialParameterAssociation::LayerParameter, 0);
	BlendDurationInfo = FMaterialParameterInfo("BlendDuration", EMaterialParameterAssociation::LayerParameter, 0);
	PreFrameOffsetInfo = FMaterialParameterInfo("PreFrameOffset", EMaterialParameterAssociation::LayerParameter, 0);
	PreNumFramesInfo = FMaterialParameterInfo("PreNumFrames", EMaterialParameterAssociation::LayerParameter, 0);
}

void UVATControlComponent::BeginPlay()
{
	Super::BeginPlay();

	//初始化Mesh
	TArray<UStaticMeshComponent*> VATMeshes;
	TArray<UActorComponent*> Components = GetOwner()->GetComponentsByTag(UStaticMeshComponent::StaticClass(), VATMeshTag);
	for (UActorComponent* ActorComponent : Components)
	{
		if (UStaticMeshComponent* VATMesh = Cast<UStaticMeshComponent>(ActorComponent))
		{
			VATMeshes.Add(VATMesh);
		}
	}
	if (VATMeshes.Num() == 0)
	{
		EXGAMEPLAY_LOG(Warning, TEXT("%s: Cannot Find VAT mesh with tag[%s]"), *FString(__FUNCTION__), *VATMeshTag.ToString());
	}

	//重新设置材质
	for (UStaticMeshComponent* VATMesh: VATMeshes)
	{
		TArray<FStaticMaterial>& StaticMaterials = VATMesh->GetStaticMesh()->GetStaticMaterials();
		for (int i = 0; i < StaticMaterials.Num(); i++)
		{
			UMaterialInstanceDynamic* MatInst = Cast<UMaterialInstanceDynamic>(StaticMaterials[i].MaterialInterface);
			if (!MatInst)
			{
				MatInst = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, StaticMaterials[i].MaterialInterface);
				if (MatInst)
				{
					VATMesh->SetMaterial(i, MatInst);
				}
			}
			if (MatInst)
			{
				MaterialInstances.Add(MatInst);
			}
		}
	}

	
}

void UVATControlComponent::PlayAnimation(const FName& AnimName, float BlendDuration)
{
	if (!AnimInfoMap.Contains(AnimName))
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s: Animation[%s] not exist"), *FString(__FUNCTION__), *AnimName.ToString());
		return;
	}

	//新的动画状态
	int FrameOffset = AnimInfoMap[AnimName].AnimStart;
	int NumFrames = AnimInfoMap[AnimName].NumFrames;
	float CurrentGameTime = UKismetSystemLibrary::GetGameTimeInSeconds(this);

	EXGAMEPLAY_LOG(Log, TEXT("%s, %s set VAT animation[%s], FrameOffset[%d], NumFrames[%d], BlendDuration[%f]"), *FString(__FUNCTION__), *GetNameSafe(GetOwner()),*AnimName.ToString(), FrameOffset, NumFrames, BlendDuration);
	for (UMaterialInstanceDynamic* MatInstDynamic : MaterialInstances)
	{
		//如果有设置Belnd， 需要获取当前动画状态
		if (CurrentAnimInfo && BlendDuration > 0.0f)
		{
			MatInstDynamic->SetScalarParameterValueByInfo(BlendFrameTimeInfo, CurrentGameTime);
			MatInstDynamic->SetScalarParameterValueByInfo(BlendDurationInfo, BlendDuration);

			MatInstDynamic->SetScalarParameterValueByInfo(PreFrameOffsetInfo, CurrentAnimInfo->AnimStart);
			MatInstDynamic->SetScalarParameterValueByInfo(PreNumFramesInfo, CurrentAnimInfo->NumFrames);
		}

		MatInstDynamic->SetScalarParameterValueByInfo(FrameOffsetInfo, FrameOffset);
		MatInstDynamic->SetScalarParameterValueByInfo(NumFramesInfo, NumFrames);
	}

	CurrentAnimInfo = &AnimInfoMap[AnimName];
}

