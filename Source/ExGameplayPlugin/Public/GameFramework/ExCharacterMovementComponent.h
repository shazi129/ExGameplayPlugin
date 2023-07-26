#pragma once
#include "GameFramework/CharacterMovementComponent.h"
#include "ExCharacterMovementComponent.generated.h"

UCLASS()
class EXGAMEPLAYPLUGIN_API UExCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UExCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void SetMovementSyncEnable(bool Value);

protected:
	virtual void CallServerMovePacked(const FSavedMove_Character* NewMove, const FSavedMove_Character* PendingMove, const FSavedMove_Character* OldMove);

private:
	uint8 bMovementSyncEnable : 1;
};