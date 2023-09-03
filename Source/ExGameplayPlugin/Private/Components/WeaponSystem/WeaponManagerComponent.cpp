#include "Components/WeaponSystem/WeaponManagerComponent.h"
#include "AbilitySystemInterface.h"

void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwnerASC();
}


UExAbilitySystemComponent* UWeaponManagerComponent::GetOwnerASC()
{
	if (!OwnerASC)
	{
		IAbilitySystemInterface* Owner = Cast<IAbilitySystemInterface>(GetOwner());
		if (Owner)
		{
			OwnerASC = Cast<UExAbilitySystemComponent>(Owner->GetAbilitySystemComponent());
		}
	}
	return OwnerASC;
}

bool UWeaponManagerComponent::HasWeapon(FGameplayTag WeaponType)
{
	for (int i = 0; i < CurrentWeaponList.Num(); i++)
	{
		if (CurrentWeaponList[i].WeaponType == WeaponType)
		{
			return true;
		}
	}
	return false;
}

FWeaponInfo* UWeaponManagerComponent::FindWeaponInfo(FGameplayTag WeaponType)
{
	for (int i = 0; i < CurrentWeaponList.Num(); i++)
	{
		if (CurrentWeaponList[i].WeaponType == WeaponType)
		{
			return &CurrentWeaponList[i];
		}
	}
	return nullptr;
}

void UWeaponManagerComponent::AddWeapon(FGameplayTag WeaponType, const FExAbilityCase& AbilityCase, AActor* WeaponActor, bool NeedAtivate)
{
/*
	if (!OwnerASC)
	{
		UE_LOG(LogTemp, Error, TEXT("UWeaponManagerComponent::AddWeapon: owner have no asc"));
		return;
	}

	if (HasWeapon(WeaponType))
	{
		RemoveWeapon(WeaponType);
	}

	FWeaponInfo* NewWeaponInfo = new(CurrentWeaponList)FWeaponInfo();
	NewWeaponInfo->WeaponType = WeaponType;
	NewWeaponInfo->WeaponGASpecHandle = OwnerASC->GiveAbilityByCase(AbilityCase, WeaponActor);
	NewWeaponInfo->WeaponActor = WeaponActor;
	if (NeedAtivate)
	{
		ActivateWeapon(WeaponType);
	}
*/
}


void UWeaponManagerComponent::ActivateWeapon(FGameplayTag WeaponType)
{
	if (!OwnerASC)
	{
		UE_LOG(LogTemp, Error, TEXT("UWeaponManagerComponent::ActivateWeapon: owner have no asc"));
		return;
	}

	FWeaponInfo* WeaponInfo = FindWeaponInfo(WeaponType);
	if (!WeaponInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("UWeaponManagerComponent::ActivateWeapon: weapon[%s] does not exist"), *WeaponType.ToString());
		return;
	}

	if (!WeaponInfo->WeaponGASpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UWeaponManagerComponent::ActivateWeapon: weapon[%s] skill have not been given"), *WeaponType.ToString());
		return;
	}
	//OwnerASC->TryActivateAbility(WeaponInfo->WeaponGASpecHandle);

	FGameplayEventData EventData;
	EventData.EventTag = WeaponInfo->WeaponType;
	EventData.Instigator = GetOwner();
	EventData.Target = WeaponInfo->WeaponActor;
	OwnerASC->TriggerAbilityFromGameplayEvent(WeaponInfo->WeaponGASpecHandle, OwnerASC->AbilityActorInfo.Get(), EventData.EventTag, &EventData, *OwnerASC);
}

void UWeaponManagerComponent::DeactivateWeapon(FGameplayTag WeaponType)
{
	FWeaponInfo* WeaponInfo = FindWeaponInfo(WeaponType);
	if (OwnerASC && WeaponInfo && WeaponInfo->WeaponGASpecHandle.IsValid())
	{
		OwnerASC->CancelAbilityHandle(WeaponInfo->WeaponGASpecHandle);
	}
}

void UWeaponManagerComponent::RemoveWeapon(FGameplayTag WeaponType)
{
	//单独拎出来，防止结束GA时又调用到RemoveWeapon
	TArray<FGameplayAbilitySpecHandle> RemovingWeaponGA;

	for (int i = CurrentWeaponList.Num() - 1; i >= 0; i--)
	{
		FWeaponInfo& WeaponInfo = CurrentWeaponList[i];
		if (WeaponInfo.WeaponType == WeaponType)
		{
			//结束GA, 并将本项移除
			if (OwnerASC && WeaponInfo.WeaponGASpecHandle.IsValid())
			{
				RemovingWeaponGA.Add(WeaponInfo.WeaponGASpecHandle);
				CurrentWeaponList.RemoveAt(i);
			}
		}
	}

	for (int i = 0; i < RemovingWeaponGA.Num(); i++)
	{
		OwnerASC->CancelAbilityHandle(RemovingWeaponGA[i]);
		OwnerASC->ClearAbility(RemovingWeaponGA[i]);
	}
}