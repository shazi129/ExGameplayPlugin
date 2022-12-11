#pragma once

#include "ExTypes.h"
#include "PerformanceLibrary.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYLIBRARY_API FScalabilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int ResolutionScale = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int ViewDistance = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int AntiAliasing = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int PostProcess = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Shadow = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int GlobalIllumination = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Reflection = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Texture = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Effects = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Foliage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Shading = 1;
};

UCLASS()
class EXGAMEPLAYLIBRARY_API UPerformanceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "PerformanceLibrary")
		static void SetScalability(FScalabilityInfo ScalabilityInfo);

	UFUNCTION(BlueprintCallable, Category = "PerformanceLibrary")
		static FScalabilityInfo GetScalability();

	UFUNCTION(BlueprintCallable, Category = "PerformanceLibrary")
		static void StartTrace(const FName Name);

	UFUNCTION(BlueprintCallable, Category = "PerformanceLibrary")
		static void StopTrace();
};