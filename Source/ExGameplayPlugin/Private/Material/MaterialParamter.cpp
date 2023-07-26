#include "Material/MaterialParamter.h"
#include "ExGameplayPluginModule.h"

FMaterialParamterConfig::FMaterialParamterConfig()
{
	ParamterName = "";
	InstancedDataIndex = -1;
	Value = 0;
}

int FInstancedMaterialParamter::GetCustomDataNum()
{
	return DefaultCustomData.Num();
}

TArray<float> FInstancedMaterialParamter::GetDefaultCustomDataData()
{
	return DefaultCustomData;
}

void FInstancedMaterialParamter::SetCustemData(TArray<float>& CustomData, int DataType, float DataValue)
{
	if (CustomData.Num() == 0)
	{
		CustomData = DefaultCustomData;
	}
	check(CustomData.Num() >= DefaultCustomData.Num());

	int* DataIndexPtr = ParamterMap.Find(DataType);
	if (!DataIndexPtr)
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s error, Invalid DataType[%d]"), *FString(__FUNCTION__), DataType);
		return;
	}

	if (!CustomData.IsValidIndex(*DataIndexPtr))
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s error, Invalid DataType[%d], DataIndex[%d]"), *FString(__FUNCTION__), DataType, *DataIndexPtr);
		return;
	}
	CustomData[*DataIndexPtr] = DataValue;
}