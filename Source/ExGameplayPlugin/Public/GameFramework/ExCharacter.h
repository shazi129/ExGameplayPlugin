#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "ExCharacter.generated.h"

UCLASS()
class EXGAMEPLAYPLUGIN_API AExCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AExCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick( float DeltaSeconds) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetRagdoll(bool Enable);


private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ability, meta = (AllowPrivateAccess = "true"))
		UAbilitySystemComponent* AbilitySystemComponent;
};