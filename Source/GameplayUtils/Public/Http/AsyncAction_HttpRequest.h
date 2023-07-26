#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_HttpRequest.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UAsyncAction_HttpRequest : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UAsyncAction_HttpRequest();
	virtual void Activate() override;

	void SetRequest(const FString& URL, const FString& Verb);

protected:
	virtual void OnRequestFinish(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	virtual void OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived);

protected:
	FHttpRequestPtr ActionHttpRequest;
};
