#pragma once

/**
 * Mass相关工具函数 
 */

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "MassProcessor.h"
#include "MassHelperLibrary.generated.h"

UCLASS()
class GAMEPLAYUTILS_API UMassHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void ExecuteProcessor(const UObject* ContextObject, UMassProcessor* Processor, const FInstancedStruct& Data);
};