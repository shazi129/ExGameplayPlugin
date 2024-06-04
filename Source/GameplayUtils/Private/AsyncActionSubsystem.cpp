// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActionSubsystem.h"
#include "GameplayUtilsModule.h"
#include "Kismet/GameplayStatics.h"

void UAsyncActionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstanceSubsystem::Initialize(Collection);
}

void UAsyncActionSubsystem::Deinitialize()
{
	UGameInstanceSubsystem::Deinitialize();
}

UAsyncActionSubsystem* UAsyncActionSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = Cast<UGameInstance>(WorldContextObject);
	if (GameInstance == nullptr)
	{
		GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	}
	if (GameInstance == nullptr)
	{
		UE_LOG(LogGameplayUtils, Error, TEXT("%s error, cannot get game instance by object[%s]"), *FString(__FUNCTION__), *GetNameSafe(WorldContextObject));
		return nullptr;
	}
	return GameInstance->GetSubsystem<UAsyncActionSubsystem>();
}

UGeneralAsyncActionObject* UAsyncActionSubsystem::CreateVoidActionObject(UObject* WorldContextObject)
{
	UGeneralAsyncActionObject* Object = NewObject<UGeneralAsyncActionObject>(WorldContextObject);
	AsyncObjectSet.Add(Object);
	return Object;
}

void UAsyncActionSubsystem::RemoveActionObject(UObject* Object)
{
	AsyncObjectSet.Remove(Object);
}
