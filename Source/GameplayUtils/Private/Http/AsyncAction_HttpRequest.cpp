#include "Http/AsyncAction_HttpRequest.h"

UAsyncAction_HttpRequest::UAsyncAction_HttpRequest()
{
	ActionHttpRequest = FHttpModule::Get().CreateRequest();
	
	ActionHttpRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncAction_HttpRequest::OnRequestFinish);
	ActionHttpRequest->OnRequestProgress().BindUObject(this, &UAsyncAction_HttpRequest::OnRequestProgress);
}

void UAsyncAction_HttpRequest::SetRequest(const FString& URL, const FString& Verb)
{
	ActionHttpRequest->SetURL(URL);
	ActionHttpRequest->SetVerb(TEXT("GET"));
}

void UAsyncAction_HttpRequest::Activate()
{
	ActionHttpRequest->ProcessRequest();
}

void UAsyncAction_HttpRequest::OnRequestFinish(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{

}

void UAsyncAction_HttpRequest::OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived)
{

}
