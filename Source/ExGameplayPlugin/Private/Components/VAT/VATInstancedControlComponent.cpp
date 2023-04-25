#include "Components/VAT/VATInstancedControlComponent.h"
#include "ExGameplayPluginModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/InstancedStaticMeshComponent.h"


void UVATInstancedControlComponent::BeginPlay()
{
	Super::BeginPlay();

	//初始化ISMC
	TArray<UActorComponent*> Components = GetOwner()->GetComponentsByTag(UInstancedStaticMeshComponent::StaticClass(), VATMeshTag);
	for (UActorComponent* Component : Components)
	{
		ISMC = Cast<UInstancedStaticMeshComponent>(Component);
		if (ISMC)
		{
			break;
		}
	}
	if (!ISMC)
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s: Cannot Find VAT instanced mesh with tag[%s], Actor[%s]"), *FString(__FUNCTION__), *VATMeshTag.ToString(), *GetNameSafe(GetOwner()));
	}

	InstancedParamter.InitParamter<EVATMaterialParamterType>(DefaultMaterialParamterConfig->ParamterMap);
	ISMC->NumCustomDataFloats = InstancedParamter.GetCustomDataNum();
}


bool UVATInstancedControlComponent::SetCustomData(int32 InstanceIndex, const TMap<EVATMaterialParamterType, float>& Values, bool bMarkRenderStateDirty)
{
	if (!ISMC)
	{
		return false;
	}

	//填充到CustomData
	TArray<float> CustomDataValues;
	for (const auto& ValueItem: Values)
	{
		InstancedParamter.SetCustemData(CustomDataValues, (int)ValueItem.Key, ValueItem.Value);
	}

	return ISMC->SetCustomData(InstanceIndex, CustomDataValues, bMarkRenderStateDirty);
}

void UVATInstancedControlComponent::PlayAnimation(int InstanceIndex, const FName& AnimName, float BlendDuration)
{
	if (!ISMC)
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s: Cannot Find VAT instanced mesh with tag[%s], Actor[%s]"), *FString(__FUNCTION__), *VATMeshTag.ToString(), *GetNameSafe(GetOwner()));
		return;
	}

	if (ISMC->NumCustomDataFloats == 0)
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s: NumCustomDataFloats==0,  tag[%s], Actor[%s]"), *FString(__FUNCTION__), *VATMeshTag.ToString(), *GetNameSafe(GetOwner()));
		return;
	}

	if (ISMC->GetInstanceCount() <= InstanceIndex)
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s: Cannot play Animation[%s] for Instance[%d], out of range[%d]"), *FString(__FUNCTION__), *AnimName.ToString(), InstanceIndex, ISMC->GetInstanceCount());
		return;
	}

	if (!AnimInfoMap.Contains(AnimName))
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s: Animation[%s] not exist"), *FString(__FUNCTION__), *AnimName.ToString());
		return;
	}

	//新的动画状态
	int FrameOffset = AnimInfoMap[AnimName].AnimStart;
	int NumFrames = AnimInfoMap[AnimName].NumFrames;
	float CurrentGameTime = UKismetSystemLibrary::GetGameTimeInSeconds(this);

	//EXGAMEPLAY_LOG(Log, TEXT("%s, %s set VAT instance[%d], animation[%s], FrameOffset[%d], NumFrames[%d], BlendDuration[%f]"), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), InstanceIndex, *AnimName.ToString(), FrameOffset, NumFrames, BlendDuration);
	
	FAnimInfo* CurrentAnimationInfo = CurrentAnimInfoMap.FindOrAdd(InstanceIndex);

	TMap<EVATMaterialParamterType, float> CustomValues;
	//如果有设置Belnd， 需要获取当前动画状态
	if (CurrentAnimationInfo && BlendDuration > 0.0f)
	{
		CustomValues.Add(EVATMaterialParamterType::E_BlendFrameTime, CurrentGameTime);
		CustomValues.Add(EVATMaterialParamterType::E_BlendDuration, BlendDuration);

		CustomValues.Add(EVATMaterialParamterType::E_PreFrameOffset, CurrentAnimationInfo->AnimStart);
		CustomValues.Add(EVATMaterialParamterType::E_PreNumFrames, CurrentAnimationInfo->NumFrames);
	}

	CustomValues.Add(EVATMaterialParamterType::E_FrameOffset, FrameOffset);
	CustomValues.Add(EVATMaterialParamterType::E_NumFrames, NumFrames);
	SetCustomData(InstanceIndex, CustomValues, true);

	CurrentAnimInfoMap[InstanceIndex] = &AnimInfoMap[AnimName];
}