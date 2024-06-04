#include "AsyncScreenshotSubsystem.h"
#include "HighResScreenshot.h"
#include "ImageUtils.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

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

UAsyncScreenshotSubsystem* UAsyncScreenshotSubsystem::GetSubsystem(UObject* ContextObject)
{
	return Cast<UAsyncScreenshotSubsystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(ContextObject, UAsyncScreenshotSubsystem::StaticClass()));
}

void UAsyncScreenshotSubsystem::TakeScreenshot(int32 Width, int32 Height, const FString& FileName)
{
	CurrentShotFileName = FileName;
	DelegateHandle = UGameViewportClient::OnScreenshotCaptured().AddUObject(this, &UAsyncScreenshotSubsystem::OnScreenshotCompleteToPng);

	FString Command = FString::Printf(TEXT("HighResShot %dx%d filename=\"%s\""), Width, Height, *FileName);
	UGameplayUtilsLibrary::ExecCommand(Command);
}

// 同步截图，异步写入磁盘，带UI
void UAsyncScreenshotSubsystem::ProcessScreenShots(const FString& InFilename, bool bInShowUI,
                                                   const FProcessScreenShotsFinishDelegate& OnFinish)
{
	UGameViewportClient* GameViewport = GEngine->GameViewport;
	FViewport* InViewport = GameViewport->Viewport;

	TArray<FColor> Bitmap;
	TSharedPtr<SWindow> WindowPtr = GameViewport->GetWindow();

	bool bScreenshotSuccessful;
	FIntVector Size(InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y, 0);
	if (bInShowUI && FSlateApplication::IsInitialized())
	{
		TSharedRef<SWidget> WindowRef = WindowPtr.ToSharedRef();
		bScreenshotSuccessful = FSlateApplication::Get().TakeScreenshot(WindowRef, Bitmap, Size);
		GScreenshotResolutionX = Size.X;
		GScreenshotResolutionY = Size.Y;
	}
	else
	{
		bScreenshotSuccessful = GetViewportScreenShot(InViewport, Bitmap);
	}

	if (bScreenshotSuccessful)
	{
		FIntRect SourceRect(0, 0, GScreenshotResolutionX, GScreenshotResolutionY);
		if (GIsHighResScreenshot)
		{
			SourceRect = GetHighResScreenshotConfig().CaptureRegion;
		}

		GetHighResScreenshotConfig().MergeMaskIntoAlpha(Bitmap, SourceRect);

		// Clip the bitmap to just the capture region if valid
		if (!SourceRect.IsEmpty())
		{
			FColor* const Data = Bitmap.GetData();
			const int32 OldWidth = Size.X;
			const int32 OldHeight = Size.Y;
			const int32 NewWidth = SourceRect.Width();
			const int32 NewHeight = SourceRect.Height();
			const int32 CaptureTopRow = SourceRect.Min.Y;
			const int32 CaptureLeftColumn = SourceRect.Min.X;

			for (int32 Row = 0; Row < NewHeight; Row++)
			{
				FMemory::Memmove(Data + Row * NewWidth, Data + (Row + CaptureTopRow) * OldWidth + CaptureLeftColumn,
				                 NewWidth * sizeof(*Data));
			}

			Bitmap.RemoveAt(NewWidth * NewHeight, OldWidth * OldHeight - NewWidth * NewHeight, false);
			Size = FIntVector(NewWidth, NewHeight, 0);
		}

		AsyncTask(ENamedThreads::AnyThread, [=]()
		{
			TArray64<uint8> PNGData;
			FImageUtils::PNGCompressImageArray(Size.X, Size.Y, Bitmap, PNGData);
			FFileHelper::SaveArrayToFile(PNGData, *InFilename);
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnFinish.ExecuteIfBound(*InFilename);
			});
		});
	}
}

void UAsyncScreenshotSubsystem::OnScreenshotCompleteToPng(int32 InWidth, int32 InHeight, const TArray<FColor>& InColors)
{
	UGameViewportClient::OnScreenshotCaptured().Remove(DelegateHandle);
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		AsyncScreenshotEndDelegate.Broadcast(CurrentShotFileName, InColors);
	});
	return;
	AsyncTask(ENamedThreads::AnyThread, [=]()
	{
		SCOPED_BOOT_TIMING("UAsyncScreenshotSubsystem::OnScreenshotCompleteToPng");
		TArray<FColor> OutPixels = InColors;
		TArray64<uint8> PNGData;
		FImageUtils::PNGCompressImageArray(InWidth, InHeight, OutPixels, PNGData);
		FFileHelper::SaveArrayToFile(PNGData, *CurrentShotFileName);
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			// AsyncScreenshotEndDelegate.Broadcast(CurrentShotFileName, InColors);
		});
	});

	// if (!CurrentShotFileName.IsEmpty())
	// {
	// 	FScopeLock SetLock(&ScreenshotDataCritical);
	//
	// 	FScreenshotData* Data = new (ScreenshotDatas)FScreenshotData();
	// 	Data->Width = InWidth;
	// 	Data->Height = InHeight;
	// 	Data->FileName = CurrentShotFileName;
	// 	Data->BitMap = InColors;
	//
	// 	if (!UseMultiThread)
	// 	{
	// 		DoProcessScreenshotData();
	// 	}
	// }
	// CurrentShotFileName.Reset();
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
	// FScopeLock SetLock(&ScreenshotDataCritical);
	// TArray<FScreenshotData> Datas = MoveTemp(ScreenshotDatas);
	// for (FScreenshotData& Data : Datas)
	// {
	// 	TArray64<uint8> CompressBitmap;
	// 	FImageUtils::PNGCompressImageArray(Data.Width, Data.Height, Data.BitMap, CompressBitmap);
	// 	FFileHelper::SaveArrayToFile(CompressBitmap, *Data.FileName);
	//
	// 	FString FileName = Data.FileName;
	//
	// 	//多线程下，通知主线程处理
	// 	if (UseMultiThread)
	// 	{
	// 		Async(EAsyncExecution::TaskGraphMainThread, [&, FileName]()
	// 		{
	// 			if (AsyncScreenshotEndDelegate.IsBound())
	// 			{
	// 				AsyncScreenshotEndDelegate.Broadcast(FileName);
	// 			}
	// 		});
	// 	}
	// 	else
	// 	{
	// 		if (AsyncScreenshotEndDelegate.IsBound())
	// 		{
	// 			AsyncScreenshotEndDelegate.Broadcast(FileName);
	// 		}
	// 	}
	// }
}