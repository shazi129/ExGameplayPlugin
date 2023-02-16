#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PackageCacheSubsystem.generated.h"

UENUM(BlueprintType)
enum EBPAsyncLoadingResult
{
	Failed				UMETA(DisplayName = "Failed"),
	Succeeded			UMETA(DisplayName = "Success"),
	Canceled			UMETA(DisplayName = "Success"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAsyncLoadPackageFinishDelegate, const FName&, PackageName, UPackage*, Package, EBPAsyncLoadingResult, Result);

UCLASS(BlueprintType)
class CACHESYSTEM_API UPackageCacheSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
		int32 LoadPackageAsync(const FName& InName);

	UFUNCTION(BlueprintCallable)
		void UnloadPackage(const FName& InName);

	UPROPERTY(BlueprintAssignable)
		FAsyncLoadPackageFinishDelegate OnAsyncLoadFinish;

private:
	void OnPackageLoadComplete(const FName& PackageName, UPackage* LevelPackage, EAsyncLoadingResult::Type Result);

private:
	TMap<FName, UPackage*> CachedPackages;
};