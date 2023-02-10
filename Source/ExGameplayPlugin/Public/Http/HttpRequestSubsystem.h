#pragma once 

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HttpRequestSubsystem.generated.h"

USTRUCT()
struct EXGAMEPLAYPLUGIN_API FDownLoadRequestInfo 
{
	GENERATED_BODY()

	FHttpRequestPtr HttpRequest;

	FString SaveFilePath;
};

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UHttpRequestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
		bool DownLoad(const FString& URL, const FString& FilePath);

	void HandleDownloadRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void HandleHttpRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived);

private:
	TMap<FString, FDownLoadRequestInfo> DownloadingRequests;
};