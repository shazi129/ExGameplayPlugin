#pragma once

#include "CoreMinimal.h"
#include "ExGameplayLibrary.h"
#include "Kismet/GameplayStatics.h"

#define DECLARE_LOG_TAG() \
private:\
	FString LogTag; \
	inline void InitLogTag(const TCHAR* ClassName) \
	{\
		this->LogTag = FString::Printf(TEXT("[%s] %s"), *(UExGameplayLibrary::GetLogPrefix(this)), ClassName);\
	}
	
#define  DEFINE_LOG_TAG(ClassName) \
	this->InitLogTag(TEXT(#ClassName))


#define DECLARE_INIT_RESULT() \
	bool bInitSuccess = true; \
	FString InitErrMsg;

#define SET_INIT_ERROR(ErrorMsg, LogCategory, LogLevel) \
	{\
		bInitSuccess = false; \
		InitErrMsg = ErrorMsg; \
		UE_LOG(LogCategory, LogLevel, TEXT("%s.%s init error: %s"), *GetName(), *FString(__FUNCTION__), *InitErrMsg);\
	}

#define  CHECK_INIT_RESULT_RETURN(LogCategory, LogLevel) \
	if (bInitSuccess == false) \
	{\
		UE_LOG(LogCategory, LogLevel, TEXT("%s.%s error, init error: %s"), *GetName(), *FString(__FUNCTION__), *InitErrMsg);\
		return; \
	}

#define  CHECK_INIT_RESULT_RETURN_VALUE(LogCategory, LogLevel, ReturnValue) \
	if (bInitSuccess == false) \
	{\
		UE_LOG(LogCategory, LogLevel, TEXT("%s.%s error, init error: %s"), *GetName(), *FString(__FUNCTION__), *InitErrMsg);\
		return ReturnValue;\
	}

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

#define DECLARE_GET_GAMEINSTANCE_SUBSYSTEM(SubsystemType, LogCategory) \
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true")) \
	static SubsystemType* GetSubsystem(const UObject* WorldContextObject) \
	{ \
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
		return GameInstance->GetSubsystem<SubsystemType>(); \
	} \

