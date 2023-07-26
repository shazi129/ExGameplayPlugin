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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncScreenshotEndDelegate, const FString&, ScreenshotFileName);

UCLASS(BlueprintType)
class UAsyncScreenshotSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
		void TakeScreenshot(int32 Width, int32 Height, const FString& FileName);

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