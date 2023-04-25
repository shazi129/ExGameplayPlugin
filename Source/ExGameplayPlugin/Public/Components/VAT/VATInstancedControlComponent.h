#pragma once

#include "Components/VAT/VATControlComponent.h"
#include "Material/MaterialParamter.h"
#include "VATInstancedControlComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UVATInstancedControlComponent : public UVATControlBaseComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void PlayAnimation(int InstanceIndex, const FName& AnimName, float BlendDuration);

	bool SetCustomData(int32 InstanceIndex, const TMap<EVATMaterialParamterType, float>& Values, bool bMarkRenderStateDirty = false);

protected:
	UInstancedStaticMeshComponent* ISMC;

	//每个Instance对应的动画，key: instanced index
	TMap<int, FAnimInfo*> CurrentAnimInfoMap;

	FInstancedMaterialParamter InstancedParamter;

};