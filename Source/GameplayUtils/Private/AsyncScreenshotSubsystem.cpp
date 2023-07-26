#include "AsyncScreenshotSubsystem.h"
#include "ImageUtils.h"
#include "ExGameplayLibrary.h"

void UAsyncScreenshotSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UseMultiThread = true;

	if (UseMultiThread)
	{
		Working = true;
		WorkThreadResult = Async(EAsyncExecution::Thread, [&]()
		{
			ProcessScreenshotData();
		});
	}	
}

void UAsyncScreenshotSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (UseMultiThread)
	{
		Working = false;
		WorkThreadResult.Get();
	}
}

void UAsyncScreenshotSubsystem::TakeScreenshot(int32 Width, int32 Height, const FString& FileName)
{
	CurrentShotFileName = FileName;
	DelegateHandle = UGameViewportClient::OnScreenshotCaptured().AddUObject(this, &UAsyncScreenshotSubsystem::OnScreenshotCompleteToPng);

	FString Command = FString::Printf(TEXT("HighResShot %dx%d filename=\"%s\""), Width, Height, *FileName);
	UExGameplayLibrary::ExecCommand(Command);
}

void UAsyncScreenshotSubsystem::OnScreenshotCompleteToPng(int32 InWidth, int32 InHeight, const TArray<FColor>& InColors)
{
	UGameViewportClient::OnScreenshotCaptured().Remove(DelegateHandle);

	if (!CurrentShotFileName.IsEmpty())
	{
		FScopeLock SetLock(&ScreenshotDataCritical);

		FScreenshotData* Data = new (ScreenshotDatas)FScreenshotData();
		Data->Width = InWidth;
		Data->Height = InHeight;
		Data->FileName = CurrentShotFileName;
		Data->BitMap = InColors;

		if (!UseMultiThread)
		{
			DoProcessScreenshotData();
		}
	}
	CurrentShotFileName.Reset();
}

void UAsyncScreenshotSubsystem::ProcessScreenshotData()
{
	while (Working)
	{
		if (ScreenshotDatas.Num() == 0)
		{
			FPlatformProcess::Sleep(0.1);
			continue;
		}

		DoProcessScreenshotData();
	}
}

void UAsyncScreenshotSubsystem::DoProcessScreenshotData()
{
	FScopeLock SetLock(&ScreenshotDataCritical);
	TArray<FScreenshotData> Datas = MoveTemp(ScreenshotDatas);
	for (FScreenshotData& Data : Datas)
	{
		TArray64<uint8> CompressBitmap;
		FImageUtils::PNGCompressImageArray(Data.Width, Data.Height, Data.BitMap, CompressBitmap);
		FFileHelper::SaveArrayToFile(CompressBitmap, *Data.FileName);

		FString FileName = Data.FileName;

		//多线程下，通知主线程处理
		if (UseMultiThread)
		{
			Async(EAsyncExecution::TaskGraphMainThread, [&, FileName]()
			{
				if (AsyncScreenshotEndDelegate.IsBound())
				{
					AsyncScreenshotEndDelegate.Broadcast(FileName);
				}
			});
		}
		else
		{
			if (AsyncScreenshotEndDelegate.IsBound())
			{
				AsyncScreenshotEndDelegate.Broadcast(FileName);
			}
		}
	}
}