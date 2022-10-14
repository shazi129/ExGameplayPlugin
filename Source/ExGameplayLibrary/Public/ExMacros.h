#pragma once

#include "CoreMinimal.h"
#include "ExGameplayLibrary.h"

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