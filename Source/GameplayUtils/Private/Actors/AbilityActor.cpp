// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AbilityActor.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

// Sets default values
AAbilityActor::AAbilityActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
}

// Called when the game starts or when spawned
void AAbilityActor::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystemComponent) 
	{
		UE_LOG(LogTemp, Log, TEXT("AAbilityActor::BeginPlay hasAuthority:%d, Ability Num: %d"), HasAuthority(), AbilityToAcquires.Num());
		//组件初始化
		if (AbilityToAcquires.Num() > 0 && HasAuthority())
		{
			for (const TSubclassOf<UGameplayAbility>& AbilityClass : AbilityToAcquires)
			{
				FGameplayAbilitySpec Spec(AbilityClass);
				AbilitySystemComponent->GiveAbility(Spec);
			}
		}
		AbilitySystemComponent->InitAbilityActorInfo(this, this); 
	}
}


void AAbilityActor::GiveAbility(const TSubclassOf<UGameplayAbility> Ability)
{
	if(AbilitySystemComponent)
	{
		if(HasAuthority() && Ability)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1));
		}
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

