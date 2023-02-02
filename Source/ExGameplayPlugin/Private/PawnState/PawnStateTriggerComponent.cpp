#include "PawnState/PawnStateTriggerComponent.h"
#include "PawnState/PawnStateLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UPawnStateTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		for (const UPawnStateAsset* Asset : PawnStateAssets)
		{
			UPawnStateLibrary::EnterPawnState(Character, FPawnStateInstance(Asset, this->GetOwner()));
		}
	}
}

void UPawnStateTriggerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		for (const UPawnStateAsset* Asset : PawnStateAssets)
		{
			UPawnStateLibrary::LeavePawnState(Character, FPawnStateInstance(Asset, this->GetOwner()));
		}
	}
}