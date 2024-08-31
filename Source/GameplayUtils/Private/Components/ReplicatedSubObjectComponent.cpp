#include "Components/ReplicatedSubObjectComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayUtilsModule.h"

UReplicatedSubObjectComponent::UReplicatedSubObjectComponent()
{
	SetIsReplicatedByDefault(true);
}

void UReplicatedSubObjectComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* OwnerActor = GetOwner();

	if (OwnerActor && OwnerActor->HasAuthority())
	{
		if (UReplicatedSubObjectsAsset* Asset = ReplicatedSubObjectsAsset.LoadSynchronous())
		{
			for (auto& ObjectClassPtr : Asset->ObjectClassList)
			{
				if (UClass* ObjectClass = ObjectClassPtr.LoadSynchronous())
				{
					AddSubObjectByClass(ObjectClass);
				}
			}
		}
	}
}

void UReplicatedSubObjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<TObjectPtr<UReplicatedSubObject>> TempSubObjectList = SubObjectList;
	for (auto& SubObject : TempSubObjectList)
	{
		SubObject->Deinitialize();
		GetOwner()->RemoveReplicatedSubObject(SubObject);
	}
	SubObjectList.Empty();

	Super::EndPlay(EndPlayReason);
}

void UReplicatedSubObjectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UReplicatedSubObjectComponent, SubObjectList, COND_None);
}

UReplicatedSubObject* UReplicatedSubObjectComponent::GetSubObjectByTag(const FString& ObjectTag)
{
	for (auto SubObject : SubObjectList)
	{
		if (SubObject && SubObject->HasTag(ObjectTag))
		{
			return SubObject;
		}
	}
	return nullptr;
}

UReplicatedSubObject* UReplicatedSubObjectComponent::GetSubObjectByClass(UClass* Class)
{
	for (auto SubObject : SubObjectList)
	{
		if (SubObject && SubObject->GetClass() == Class)
		{
			return SubObject;
		}
	}
	return nullptr;
}

UReplicatedSubObject* UReplicatedSubObjectComponent::AddSubObjectByClass(TSubclassOf<UReplicatedSubObject> SubObjectClass)
{
	if (!SubObjectClass)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, SubObjectClass is null"), *FString(__FUNCTION__));
		return nullptr;
	}

	//TODO: 存疑：需不需要禁止同一类型的SubObject
	for (auto SubObject : SubObjectList)
	{
		if (SubObject && SubObject->GetClass() == SubObjectClass)
		{
			return SubObject;
		}
	}

	AActor* Owner = GetOwner();
	if (UReplicatedSubObject* SubObject = NewObject<UReplicatedSubObject>(Owner, SubObjectClass))
	{
		SubObject->Initialize();
		Owner->AddReplicatedSubObject(SubObject);
		SubObjectList.Add(SubObject);
		return SubObject;
	}

	GAMEPLAYUTILS_LOG(Error, TEXT("%s error, create subobject failed"), *FString(__FUNCTION__));
	return nullptr;
}

void UReplicatedSubObjectComponent::RemoveSubObject(UReplicatedSubObject* SubObject)
{
	int RemoveNum = SubObjectList.Remove(SubObject);
	if (SubObject && RemoveNum > 0)
	{
		SubObject->Deinitialize();
		GetOwner()->RemoveReplicatedSubObject(SubObject);
	}
}
