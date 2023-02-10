#include "Http/AsyncAction_HttpDownloadFile.h"
#include "ExGameplayPluginModule.h"

UAsyncAction_HttpDownloadFile* UAsyncAction_HttpDownloadFile::AsyncHttpDownloadFile(const FString& URL, const FString& FilePath)
{
	UAsyncAction_HttpDownloadFile* AsyncAction = NewObject<UAsyncAction_HttpDownloadFile>();
	AsyncAction->SetRequest(URL, TEXT("GET"));
	AsyncAction->SavedFilePath = FilePath;
	return AsyncAction;
}

void UAsyncAction_HttpDownloadFile::OnRequestFinish(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	FHttpDownloadFileResult Result;
	Result.Finished = true;
	Result.SavedFilePath = SavedFilePath;
	
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid())
	{
		Result.Success = false;
	}
	else
	{
		Result.ResponseCode = HttpResponse->GetResponseCode();
		if (bSucceeded && EHttpResponseCodes::IsOk(Result.ResponseCode))
		{
			EXGAMEPLAY_LOG(Log, TEXT("%s Download Finish, size[%d], URL[%s]"), *FString(__FUNCTION__), HttpResponse->GetContentLength(), *ActionHttpRequest->GetURL());
			Result.ResponseSize = HttpResponse->GetContentLength();
			FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *Result.SavedFilePath);
		}
	}

	if (OnFinish.IsBound())
	{
		OnFinish.Broadcast(Result);
	}
}

void UAsyncAction_HttpDownloadFile::OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived)
{
	if (OnProgress.IsBound())
	{
		FHttpDownloadFileResult Result;
		Result.Finished = false;
		Result.ResponseSize = BytesReceived;
		OnProgress.Broadcast(Result);
	}
}