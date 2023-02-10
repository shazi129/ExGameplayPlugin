#include "FileHelperLibrary.h"
#include "ExGameplayLibraryModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HAL/Platform.h"

bool UFileHelperLibrary::FileExists(const FString& Filename)
{
	FString AbsFilePath = UKismetSystemLibrary::ConvertToAbsolutePath(Filename);
	return FPlatformFileManager::Get().GetPlatformFile().FileExists(*AbsFilePath);
}

int64 UFileHelperLibrary::FileSize(const FString& Filename)
{
	FString AbsFilePath = UKismetSystemLibrary::ConvertToAbsolutePath(Filename);
	return FPlatformFileManager::Get().GetPlatformFile().FileSize(*AbsFilePath);
}

bool UFileHelperLibrary::DeleteFile(const FString& Filename)
{
	FString AbsFilePath = UKismetSystemLibrary::ConvertToAbsolutePath(Filename);
	return FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*AbsFilePath);
}

bool UFileHelperLibrary::LoadFileToArray(TArray<uint8>& Result, const FString& FileName, int Flags)
{
	return FFileHelper::LoadFileToArray(Result, *FileName, Flags);
}

bool UFileHelperLibrary::SaveArrayToFile(TArray<uint8>& Result, const FString& FileName)
{
	return FFileHelper::SaveArrayToFile(Result, *FileName);
}

void UFileHelperLibrary::OpenFolder(const FString& FolderPath)
{
	if (FolderPath.IsEmpty())
	{
		EXLIBRARY_LOG(Error, TEXT("UFileHelperLibrary::OpenFolder error, Empty Path"));
		return;
	}

#if PLATFORM_WINDOWS
	FString AbsolutePath = UKismetSystemLibrary::ConvertToAbsolutePath(FolderPath);
	AbsolutePath = AbsolutePath.Replace(TEXT("/"), TEXT("\\"));
	FString Command = FString::Printf(TEXT("explorer %s"), *AbsolutePath);

	EXLIBRARY_LOG(Log, TEXT("UFileHelperLibrary::OpenFolder, Command[%s]"), *Command);

	system(TCHAR_TO_UTF8(*Command));
#else
	EXLIBRARY_LOG(Warning, TEXT("UFileHelperLibrary::OpenFolder warning, API OpenFolder Only supported Windows"));
#endif
}

bool UFileHelperLibrary::CreateFolder(const FString& FolderPath)
{
	FString AbsFolderPath = UKismetSystemLibrary::ConvertToAbsolutePath(FolderPath);

	auto IsSlashOrBackslash = [](TCHAR C) { return C == TEXT('/') || C == TEXT('\\'); };
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	//找到最近存在的目录
	FString LastExistPath = AbsFolderPath;
	TArray<FString> SubPaths;

	while (!PlatformFile.DirectoryExists(*LastExistPath))
	{
		int32 EndPos = LastExistPath.FindLastCharByPredicate(IsSlashOrBackslash);
		LastExistPath = LastExistPath.Left(EndPos);
		SubPaths.Add(LastExistPath.Right(EndPos));
	}

	int32 Pos = AbsFolderPath.FindLastCharByPredicate(IsSlashOrBackslash);
	if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*AbsFolderPath))
	{
		EXLIBRARY_LOG(Log, TEXT("%s create directory[%s] error"), *FString(__FUNCTION__), *AbsFolderPath);
		return false;
	}
	return true;
}

bool UFileHelperLibrary::CreateDirectoryTree(const FString& Path)
{
	FString AbsPath = UKismetSystemLibrary::ConvertToAbsolutePath(Path);
	return FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*AbsPath);
}

bool UFileHelperLibrary::DeleteDirectoryRecursively(const FString& Path)
{
	FString AbsPath = UKismetSystemLibrary::ConvertToAbsolutePath(Path);
	return FPlatformFileManager::Get().GetPlatformFile().DeleteDirectoryRecursively(*AbsPath);
}

bool UFileHelperLibrary::DirectoryExists(const FString& Path)
{
	FString AbsPath = UKismetSystemLibrary::ConvertToAbsolutePath(Path);
	return FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*AbsPath);
}
