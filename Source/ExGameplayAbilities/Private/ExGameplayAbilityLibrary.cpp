#include "ExGameplayAbilityLibrary.h"
#include "ExGameplayAbilityTargetTypes.h"

TArray<int64> UExGameplayAbilityLibrary::GetInt64ArrayFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index)
{
	TArray<int64> Result;

	if (Index >= 0 && TargetData.Num() > Index)
	{
		const FGameplayAbilityTargetData_Int64Array* Int64ArrayData = static_cast<const FGameplayAbilityTargetData_Int64Array*>(TargetData.Get(Index));
		if (Int64ArrayData != nullptr)
		{
			Result = Int64ArrayData->Int64Array;
		}
	}

	return Result;

}

FInstancedStruct UExGameplayAbilityLibrary::GetInstancedStructFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int Index)
{
	if (Index >= 0 && TargetData.Num() > Index)
	{
		const FGameplayAbilityTargetData_FInstancedStruct* InstancedStruct = static_cast<const FGameplayAbilityTargetData_FInstancedStruct*>(TargetData.Get(Index));
		if (InstancedStruct != nullptr)
		{
			return InstancedStruct->InstancedStruct;
		}
	}
	return FInstancedStruct();
}