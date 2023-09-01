#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"

#ifndef GET_GAMEINSTANCE_SUBSYSTEM

#define  GET_GAMEINSTANCE_SUBSYSTEM(LogCategory, SubsystemType, WorldContextObject) \
	const UGameInstance* GameInstance = Cast<UGameInstance>(WorldContextObject); \
	if (GameInstance == nullptr) \
	{ \
		GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject); \
	} \
	if (GameInstance == nullptr) \
	{ \
		UE_LOG(LogCategory, Error, TEXT("%s error, cannot get game instance by object[%s]"), *FString(__FUNCTION__), *GetNameSafe(WorldContextObject)); \
		return nullptr; \
	} \
	return GameInstance->GetSubsystem<SubsystemType>();\

#endif

#ifndef DECLARE_GET_GAMEINSTANCE_SUBSYSTEM

#define DECLARE_GET_GAMEINSTANCE_SUBSYSTEM(SubsystemType, LogCategory) \
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true")) \
	static SubsystemType* GetSubsystem(const UObject* WorldContextObject) \
	{ \
		GET_GAMEINSTANCE_SUBSYSTEM(LogCategory, SubsystemType, WorldContextObject) \
	} \

#endif

#ifndef GET_WORLD_SUBSYSTEM

#define  GET_WORLD_SUBSYSTEM(LogCategory, SubsystemType, WorldContextObject) \
	const UWorld* World = Cast<UWorld>(WorldContextObject); \
	if (World == nullptr && WorldContextObject != nullptr) \
	{ \
		World = WorldContextObject->GetWorld(); \
	} \
	if (World == nullptr) \
	{ \
		UE_LOG(LogCategory, Error, TEXT("%s error, cannot get world by object[%s]"), *FString(__FUNCTION__), *GetNameSafe(WorldContextObject)); \
		return nullptr; \
	} \
	return World->GetSubsystem<SubsystemType>();\

#endif

#ifndef DECLARE_GET_WORLD_SUBSYSTEM

#define DECLARE_GET_WORLD_SUBSYSTEM(SubsystemType, LogCategory) \
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true")) \
	static SubsystemType* GetSubsystem(const UObject* WorldContextObject) \
	{ \
		GET_WORLD_SUBSYSTEM(LogCategory, SubsystemType, WorldContextObject) \
	} \

#endif