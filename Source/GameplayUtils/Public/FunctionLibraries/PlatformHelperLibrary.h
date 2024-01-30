#pragma once

#include "PlatformHelperLibrary.generated.h"

//操作系统类型
UENUM(BlueprintType)
enum class EPlatformType : uint8
{
	E_Windows			UMETA(DisplayName = "Windows"),
	E_Android			UMETA(DisplayName = "Android"),
	E_IOS				UMETA(DisplayName = "iOS"),
	E_Unknow,
};

UCLASS()
class GAMEPLAYUTILS_API UPlatformHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Platform Helper")
		static FString GetComputerName();

	UFUNCTION(BlueprintPure, Category = "Platform Helper")
		static FString GetPrimaryGPUBrand();

	UFUNCTION(BlueprintPure, Category = "Platform Helper")
		static FString GetCPUBrand();

	UFUNCTION(BlueprintPure, Category = "Platform Helper")
		static FORCEINLINE double Seconds() { return FPlatformTime::Seconds(); }

	UFUNCTION(BlueprintCallable, Category = "Platform Helper")
		static void ClipboardCopy(const FString& Value, bool LogValue=false);

	UFUNCTION(BlueprintPure, Category="Platform Helper")
		static EPlatformType GetPlatformType();

	UFUNCTION(BlueprintPure, Category = "Platform Helper")
		static FString GetIOSApplicationID();
};