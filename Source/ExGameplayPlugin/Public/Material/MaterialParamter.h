#pragma once

#include "CoreMinimal.h"
#include "MaterialParamter.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FMaterialParamterConfig
{
	GENERATED_BODY()

	//参数名
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName ParamterName;

	//Instance Index
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int InstancedDataIndex;

	//默认值
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Value;

	FMaterialParamterConfig();
};

USTRUCT()
struct EXGAMEPLAYPLUGIN_API FInstancedMaterialParamter
{
	GENERATED_BODY()

private:
	//某个材质默认的CustomData
	TArray<float> DefaultCustomData;

	//某个材质参数类型到index的映射，key: 类型，可以引申为枚举，value：参数在CustomData中的下标
	TMap<int, int> ParamterMap;

public:
	//从某个配置读入
	template<typename Enum>
	void InitParamter(TMap<Enum, FMaterialParamterConfig> ParamterConfig)
	{
		int MaxDataIndex = -1; //获取Custom Data数量
		for (auto& ParamterItem : ParamterConfig)
		{
			if (ParamterItem.Value.InstancedDataIndex > MaxDataIndex)
			{
				MaxDataIndex = ParamterItem.Value.InstancedDataIndex;
			}
		}

		DefaultCustomData.Reset();
		DefaultCustomData.Init(0, MaxDataIndex + 1);

		//初始化默认值
		for (auto& ParamterItem : ParamterConfig)
		{
			DefaultCustomData[ParamterItem.Value.InstancedDataIndex] = ParamterItem.Value.Value;
			ParamterMap.Add((int)ParamterItem.Key, ParamterItem.Value.InstancedDataIndex);
		}
	}

	int GetCustomDataNum();

	TArray<float> GetDefaultCustomDataData();
	void SetCustemData(TArray<float>& CustomData, int DataType, float DataValue);
};