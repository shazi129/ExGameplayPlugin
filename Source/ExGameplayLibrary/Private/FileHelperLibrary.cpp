#include "FileHelperLibrary.h"
#include "ExGameplayLibraryModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HAL/Platform.h"

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