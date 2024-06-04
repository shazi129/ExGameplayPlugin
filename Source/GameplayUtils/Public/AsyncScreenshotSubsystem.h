#pragma once

#include "CoreMinimal.h"
#include "AsyncScreenshotSubsystem.generated.h"

USTRUCT()
struct FScreenshotData
{
	GENERATED_BODY()

	int32 Width;
	int32 Height;

	TArray<FColor> BitMap;

	FString FileName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncScreenshotEndDelegate, const FString&, ScreenshotFileName, const TArray<FColor>&, InColors);
DECLARE_DYNAMIC_DELEGATE_OneParam(FProcessScreenShotsFinishDelegate, const FString&, ScreenshotFileName);

UCLASS(BlueprintType)
class UAsyncScreenshotSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	static UAsyncScreenshotSubsystem* GetSubsystem(UObject* ContextObject);
	
	UFUNCTION(BlueprintCallable)
	void TakeScreenshot(int32 Width, int32 Height, const FString& FileName);

	UFUNCTION(BlueprintCallable)
	void ProcessScreenShots(const FString& InFilename,bool bInShowUI, const FProcessScreenShotsFinishDelegate& OnFinish);
	
	void OnScreenshotCompleteToPng(int32 InWidth, int32 InHeight, const TArray<FColor>& InColors);

	void ProcessScreenshotData();

private:
	void DoProcessScreenshotData();

public:
	UPROPERTY(BlueprintAssignable)
	FAsyncScreenshotEndDelegate AsyncScreenshotEndDelegate;

private:
	FDelegateHandle DelegateHandle;
	FString CurrentShotFileName;

	TArray<FScreenshotData> ScreenshotDatas;
	FCriticalSection ScreenshotDataCritical;
	TFuture<void> WorkThreadResult;

	bool Working;

	bool UseMultiThread;
};