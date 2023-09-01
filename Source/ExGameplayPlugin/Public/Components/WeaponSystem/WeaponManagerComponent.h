#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "ExAbilitySystemComponent.h"
#include "WeaponManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()

	//武器类型, key
	UPROPERTY(BlueprintReadOnly)
		FGameplayTag WeaponType;

	//武器对应Actor
	UPROPERTY(BlueprintReadOnly)
		AActor* WeaponActor;

	//武器对应技能
	UPROPERTY(BlueprintReadOnly)
		FGameplayAbilitySpecHandle WeaponGASpecHandle;
};

UCLASS(ClassGroup = (Weapon), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class UWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	TArray<FWeaponInfo> CurrentWeaponList;

	UExAbilitySystemComponent* OwnerASC;

	const FGameplayAbilitySpecHandle InvalidGASpedHandle;

public:
	//获取owner身上的ASC组件
	UFUNCTION(BlueprintCallable)
		UExAbilitySystemComponent* GetOwnerASC();

	UFUNCTION(BlueprintCallable)
		bool HasWeapon(FGameplayTag WeaponType);

	FWeaponInfo* FindWeaponInfo(FGameplayTag WeaponType);

	UFUNCTION(BlueprintCallable)
		void AddWeapon(FGameplayTag WeaponType, const FExAbilityCase& AbilityCase, AActor* WeaponActor,  bool NeedAtivate=false);

	UFUNCTION(BlueprintCallable)
		void ActivateWeapon(FGameplayTag WeaponType);

	UFUNCTION(BlueprintCallable)
		void DeactivateWeapon(FGameplayTag WeaponType);

	UFUNCTION(BlueprintCallable)
		void RemoveWeapon(FGameplayTag WeaponType);
};