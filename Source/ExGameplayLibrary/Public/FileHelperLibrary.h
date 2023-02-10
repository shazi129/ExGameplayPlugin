//Blueprint Library for FFileHelper


#pragma once

#include "FileHelperLibrary.generated.h"

UCLASS()
class EXGAMEPLAYLIBRARY_API UFileHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//文件是否存在
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool FileExists(const FString& Filename);

	//文件大小
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static int64 FileSize(const FString& Filename);

	//删除文件
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool	 DeleteFile(const FString& Filename);

	//读文件
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool LoadFileToArray(TArray<uint8>& Result, const FString& FileName, int Flags);
	
	//保存文件
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool SaveArrayToFile(TArray<uint8>& Result, const FString& FileName);

	//打开文件夹
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static void OpenFolder(const FString& FolderPath);

	//创建文件夹
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool CreateFolder(const FString& FolderPath);

	//创建文件夹
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool CreateDirectoryTree(const FString& Path);

	//删除文件夹
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool DeleteDirectoryRecursively(const FString& Path);

	//判断文件夹是否存在
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool DirectoryExists(const FString& Path);
};