#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_UnzipFile.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FUnzipFileResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly);
	bool Finished;

	UPROPERTY(BlueprintReadOnly);
	bool Success;

	UPROPERTY(BlueprintReadOnly);
	int TotalFileCount;

	UPROPERTY(BlueprintReadOnly);
	int CurrentFileIndex;

	UPROPERTY(BlueprintReadOnly);
	FString CurrentFileName;

	FUnzipFileResult() 
	{
		Finished = false;
		Success = false;
		TotalFileCount = 0;
		CurrentFileIndex = 0;
		CurrentFileName = "";
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnzipFileResultDelegate, const FUnzipFileResult&, Result);

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UAsyncAction_UnzipFile : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UAsyncAction_UnzipFile();

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UAsyncAction_UnzipFile* AsyncUnzipFile(const FString& ZipFilePath, const FString& DestPath);

	virtual void Activate() override;

private:
	void UnzipFile();

	void OnProgressError(FUnzipFileResult& Result);
	void OnFinishError(FUnzipFileResult& Result);

public:
	UPROPERTY(BlueprintAssignable)
		FUnzipFileResultDelegate OnFinish;

	UPROPERTY(BlueprintAssignable)
		FUnzipFileResultDelegate OnProgress;

private:
	FString ZipFilePath;
	FString DestPath;
};