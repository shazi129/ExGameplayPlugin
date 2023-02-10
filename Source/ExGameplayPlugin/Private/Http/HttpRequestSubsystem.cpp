#include "Http/HttpRequestSubsystem.h"
#include "ExGameplayPluginModule.h"

void UHttpRequestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UHttpRequestSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UHttpRequestSubsystem::DownLoad(const FString& URL, const FString& FilePath)
{
	if (DownloadingRequests.Contains(URL))
	{
		EXGAMEPLAY_LOG(Warning, TEXT("%s Warning, URL[%s] is downloading"), *FString(__FUNCTION__), *URL);
		return true;
	}


	FDownLoadRequestInfo& RequestInfo = DownloadingRequests.Add(URL);
	RequestInfo.SaveFilePath = FilePath;
	RequestInfo.HttpRequest = FHttpModule::Get().CreateRequest();

	RequestInfo.HttpRequest->SetURL(URL);
	RequestInfo.HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpRequestSubsystem::HandleDownloadRequest);
	RequestInfo.HttpRequest->OnRequestProgress().BindUObject(this, &UHttpRequestSubsystem::HandleHttpRequestProgress);
	RequestInfo.HttpRequest->SetVerb(TEXT("GET"));
	RequestInfo.HttpRequest->ProcessRequest();

	return true;
}

void UHttpRequestSubsystem::HandleDownloadRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) 
{
	FString RequstURL = HttpRequest->GetURL();
	if (!DownloadingRequests.Contains(RequstURL))
	{
		EXGAMEPLAY_LOG(Warning, TEXT("%s Warning, unknow http respones URL[%s], ignore it"), *FString(__FUNCTION__), *RequstURL);
		return;
	}

	FDownLoadRequestInfo RequestInfo;
	DownloadingRequests.RemoveAndCopyValue(RequstURL, RequestInfo);

	if (!HttpRequest.IsValid() || !HttpResponse.IsValid()) 
	{
		return;
	}

	//200代表成功
	int32 responseCode = HttpResponse->GetResponseCode();
	if (bSucceeded && EHttpResponseCodes::IsOk(responseCode)) 
	{
		EXGAMEPLAY_LOG(Log, TEXT("%s Download size:%d"), *FString(__FUNCTION__), HttpResponse->GetContentLength());
		FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *RequestInfo.SaveFilePath);
	}
}

void UHttpRequestSubsystem::HandleHttpRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived)
{
	EXGAMEPLAY_LOG(Warning, TEXT("%s, HttpRequest[%s], BytesSend[%d], BytesReceived[%d]"), *FString(__FUNCTION__), *HttpRequest->GetURL(), BytesSent, BytesReceived);
}
