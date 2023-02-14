#include "Zip/AsyncAction_UnzipFile.h"
#include "ExGameplayPluginModule.h"
#include "Zip/minizip/unzip.h"
#include "Zip/MiniCodeConvert.h"
#include "HAL/Platform.h"

UAsyncAction_UnzipFile::UAsyncAction_UnzipFile()
{

}

UAsyncAction_UnzipFile* UAsyncAction_UnzipFile::AsyncUnzipFile(const FString& ZipFilePath, const FString& DestPath)
{
	UAsyncAction_UnzipFile* AsyncAction = NewObject<UAsyncAction_UnzipFile>();
	AsyncAction->ZipFilePath = ZipFilePath;
	AsyncAction->DestPath = DestPath;
	return AsyncAction;
}

void UAsyncAction_UnzipFile::Activate()
{
	Async(EAsyncExecution::Thread, [&]()
		{
			UnzipFile();
		});
}

void UAsyncAction_UnzipFile::UnzipFile()
{
	FUnzipFileResult Result;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*DestPath) && !PlatformFile.CreateDirectoryTree(*DestPath))
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s error, cannot create dest dir: %s"), *FString(__FUNCTION__), *DestPath);
		OnFinishError(Result);
		return;
	}

	//1. 打开zip文件
	const unzFile& ZFile = unzOpen64(FMiniCodeConvert::WideCharToMultiByte(*ZipFilePath).c_str());
	if (ZFile == nullptr)
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s error, cannot open zip file: %s"), *FString(__FUNCTION__), *ZipFilePath);
		OnFinishError(Result);
		return;
	}

	// 2. 读取zip文件信息
	unz_global_info64 ZGlobalInfo;
	unzGetGlobalInfo64(ZFile, &ZGlobalInfo);

	// 4. 解压后的信息
	unz_file_info64 ZFileInfo;

	const int NAME_BUFF_SIZE = 512;
	const int FILE_DATA_INIT_BUFF_SIZE = 512 * 1024;
	char* Filename = new char[NAME_BUFF_SIZE];
	char* FileData = new char[FILE_DATA_INIT_BUFF_SIZE];

	int ReadLength = 0;

	Result.TotalFileCount = ZGlobalInfo.number_entry;
	for (int i = 0; i < ZGlobalInfo.number_entry; ++i)
	{
		Result.CurrentFileIndex = i + 1;
		Result.CurrentFileName = "";
		Result.Finished = false;

		if (i > 0)
		{
			unzCloseCurrentFile(ZFile);
			unzGoToNextFile(ZFile);
		}

		if (unzGetCurrentFileInfo64(ZFile, &ZFileInfo, Filename, NAME_BUFF_SIZE, nullptr, 0, nullptr, 0) != UNZ_OK)
		{
			EXGAMEPLAY_LOG(Error, TEXT("%s error, cannot open zip file: %s"), *FString(__FUNCTION__), *ZipFilePath);
			OnProgressError(Result);
		}

		if (unzOpenCurrentFile(ZFile) != UNZ_OK)
		{
			EXGAMEPLAY_LOG(Error, TEXT("%s error, cannot open zip file: %s"), *FString(__FUNCTION__), *ZipFilePath);
			OnProgressError(Result);
			continue;
		}

		FString FullFilePath = DestPath / FMiniCodeConvert::MultiByteToWideChar(Filename);
		Result.CurrentFileName = FullFilePath;

		//如果是文件夹
		if (ZFileInfo.uncompressed_size == 0)
		{
			if (FullFilePath.EndsWith(TEXT("/")) || FullFilePath.EndsWith(TEXT("\\")))
			{
				if (!IFileManager::Get().MakeDirectory(*FullFilePath, true))
				{
					EXGAMEPLAY_LOG(Error, TEXT("%s error, MakeDirectory: %s"), *FString(__FUNCTION__), *FullFilePath);
					OnProgressError(Result);
					continue;
				}
			}
			else
			{
				if (!FFileHelper::SaveStringToFile(TEXT(""), *FullFilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
				{
					EXGAMEPLAY_LOG(Error, TEXT("%s error, SaveStringToFile: %s"), *FString(__FUNCTION__), *FullFilePath);
					OnProgressError(Result);
					continue;
				}
			}
		}
		else
		{
			TSharedPtr<FArchive> ArchivePtr = TSharedPtr<FArchive>(IFileManager::Get().CreateFileWriter(*FullFilePath));
			

			if (ArchivePtr != nullptr)
			{
				const double UnCompressRadio = ((double)ZFileInfo.compressed_size) / ZFileInfo.uncompressed_size;
				const int BlockCount = (ZFileInfo.uncompressed_size + FILE_DATA_INIT_BUFF_SIZE - 1) / FILE_DATA_INIT_BUFF_SIZE;
				for (int Block = 0; Block < BlockCount; ++Block)
				{
					ReadLength = unzReadCurrentFile(ZFile, FileData, FILE_DATA_INIT_BUFF_SIZE);
					ArchivePtr->Serialize(FileData, ReadLength);

				}

				if (!ArchivePtr->Close())
				{
					EXGAMEPLAY_LOG(Error, TEXT("%s error, Close File: %s"), *FString(__FUNCTION__), *FullFilePath);
					OnProgressError(Result);
					continue;
				}
			}
			else
			{
				EXGAMEPLAY_LOG(Error, TEXT("%s error, Create File: %s"), *FString(__FUNCTION__), *FullFilePath);
				OnProgressError(Result);
				continue;
			}

			if (OnProgress.IsBound())
			{
				OnProgress.Broadcast(Result);
			}
		}
	}

	unzClose(ZFile);

	Result.Finished = true;
	Result.Success = true;
	Result.CurrentFileName = "";
	if (OnFinish.IsBound())
	{
		OnFinish.Broadcast(Result);
	}
}

void UAsyncAction_UnzipFile::OnProgressError(FUnzipFileResult& Result)
{
	Result.Finished = false;
	Result.Success = false;
	if (OnProgress.IsBound())
	{
		OnProgress.Broadcast(Result);
	}
}

void UAsyncAction_UnzipFile::OnFinishError(FUnzipFileResult& Result)
{
	Result.Finished = true;
	Result.Success = false;
	if (OnFinish.IsBound())
	{
		OnFinish.Broadcast(Result);
	}
}