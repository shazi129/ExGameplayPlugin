#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ZipFile.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UAsyncAction_ZipFile : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UAsyncAction_ZipFile();
};