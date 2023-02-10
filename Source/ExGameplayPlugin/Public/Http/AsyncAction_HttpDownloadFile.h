#pragma once

#include "Http/AsyncAction_HttpRequest.h"
#include "AsyncAction_HttpDownloadFile.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FHttpDownloadFileResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly);
	bool Success;

	UPROPERTY(BlueprintReadOnly);
	bool Finished;

	UPROPERTY(BlueprintReadOnly);
	int ResponseCode;

	UPROPERTY(BlueprintReadOnly);
	int ResponseSize;

	UPROPERTY(BlueprintReadOnly);
	FString SavedFilePath;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHttpDownloadFileResultDelegate, const FHttpDownloadFileResult&, Result);

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UAsyncAction_HttpDownloadFile : public UAsyncAction_HttpRequest
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UAsyncAction_HttpDownloadFile* AsyncHttpDownloadFile(const FString& URL, const FString& FilePath);

protected:
	virtual void OnRequestFinish(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) override;
	virtual void OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived) override;

public:
	UPROPERTY(BlueprintAssignable)
		FHttpDownloadFileResultDelegate OnFinish;

	UPROPERTY(BlueprintAssignable)
		FHttpDownloadFileResultDelegate OnProgress;

private:
	FString SavedFilePath;
};
