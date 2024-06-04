// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "AbilityActor.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API AAbilityActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbilityActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ability System")
	UAbilitySystemComponent* AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return AbilitySystemComponent;}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability System")
	TArray<TSubclassOf<class UGameplayAbility>> AbilityToAcquires;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category="Ability System")
	void GiveAbility(const TSubclassOf<UGameplayAbility> Ability);

};
