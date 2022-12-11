#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetInteractActions/WidgetInteractAction.h"
#include "ExecCmdAction.generated.h"

UCLASS()
class EXUMG_API UExecCmdAction : public UWidgetInteractAction
{
	GENERATED_BODY()

public:
	virtual void NativeExecute() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Command;
};