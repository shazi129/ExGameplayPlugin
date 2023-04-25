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

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetResolutionQuality(int ResolutionScale);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetViewDistanceQuality(int ViewDistanceQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetAntiAliasingQuality(int AntiAliasingQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetPostProcessQuality(int PostProcessQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetShadowQuality(int ShadowQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetGlobalIlluminationQuality(int GlobalIlluminationQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetReflectionQuality(int ReflectionQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetTextureQuality(int TextureQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetEffectsQuality(int EffectsQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetFoliageQuality(int FoliageQuality);

	UFUNCTION(BlueprintCallable, Category="PerformanceLibrary")
		static void SetShadingQuality(int ShadingQuality);
	
};