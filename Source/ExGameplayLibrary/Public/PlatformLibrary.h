#pragma once

#include "PlatformLibrary.generated.h"


USTRUCT(BlueprintType)
struct EXGAMEPLAYLIBRARY_API FMemoryInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int64 TotalPhysical;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int64 TotalVirtual;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 TotalPhysicalGB;
};


UCLASS()
class EXGAMEPLAYLIBRARY_API UPlatformLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "PlatformLibrary")
		static FString GetComputerName();

	UFUNCTION(BlueprintPure, Category = "PlatformLibrary")
		static FString GetPrimaryGPUBrand();

	UFUNCTION(BlueprintPure, Category = "PlatformLibrary")
		static FString GetCPUBrand();

	UFUNCTION(BlueprintPure, Category = "PlatformLibrary")
		static FMemoryInfo GetMemoryInfo();
};