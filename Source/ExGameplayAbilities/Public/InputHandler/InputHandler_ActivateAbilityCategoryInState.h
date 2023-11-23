#pragma once

#include "InputHandler/InputHandler_ActivateAbilityByTag.h"
#include "InputHandler_ActivateAbilityCategoryInState.generated.h"

USTRUCT()
struct EXGAMEPLAYABILITIES_API FPawnStateAbilityCategory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayThumbnail = "false"))
	TSoftObjectPtr<UPawnStateAsset> PawnState;

	UPROPERTY(EditAnywhere)
	FGameplayTag CategoryTag;
};

USTRUCT()
struct EXGAMEPLAYABILITIES_API FActivateAbilityCategoryData : public FInputActionHandlerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag DefaultAbilityCatory;

	UPROPERTY(EditAnywhere)
	TArray<FPawnStateAbilityCategory> CategoryList;

	//是否需要将这个行为发送到server端， 默认在主控端执行
	UPROPERTY(EditAnywhere)
	bool ToServer = false;
};

UCLASS()
class EXGAMEPLAYABILITIES_API UInputHandler_ActivateAbilityCategoryInState : public UInputActionHandler
{
	GENERATED_BODY()

public:
	virtual void NativeExecute(const FInputActionValue& inputValue) override;
};