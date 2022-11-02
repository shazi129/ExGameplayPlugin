#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ExButton.generated.h"

UCLASS()
class EXUMG_API UExButton : public UButton
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnRebuildWidget();
};