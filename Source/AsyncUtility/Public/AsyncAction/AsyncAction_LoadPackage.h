#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_LoadPackage.generated.h"

UENUM(BlueprintType)
enum EBPAsyncLoadingResult
{
	Failed				UMETA(DisplayName = "Failed"),
	Succeeded			UMETA(DisplayName = "Success"),
	Canceled			UMETA(DisplayName = "Success"),
};

USTRUCT(BlueprintType)
struct ASYNCUTILITY_API FAsyncLoadPackageResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly);
	TEnumAsByte<EBPAsyncLoadingResult> Result;

	UPROPERTY(BlueprintReadOnly);
	FName PackageName;

	UPROPERTY(BlueprintReadOnly);
	UPackage* Package;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadPackageAsyncResultDelegate, const FAsyncLoadPackageResult&, Result);

UCLASS(BlueprintType)
class ASYNCUTILITY_API UAsyncAction_LoadPackage : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	static EBPAsyncLoadingResult ConvertEAsyncLoadingResult(EAsyncLoadingResult::Type Result);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncAction_LoadPackage* AsyncLoadPackage(const FString& PackageName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void UnLoadPackage(UPackage* Package);

	virtual void Activate() override;

protected:
	void OnPackageLoadFinish(const FName& InPackageName, UPackage* LevelPackage, EAsyncLoadingResult::Type InResult);

public:
	UPROPERTY(BlueprintAssignable)
		FLoadPackageAsyncResultDelegate OnFinish;

private:
	FString PackageName;
};