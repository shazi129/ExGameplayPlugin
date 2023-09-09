#pragma once

#include "CoreMinimal.h"
#include "Engine/WorldComposition.h"
#include "ObjectTools.h"
#include "ExEditorEngineLibrary.generated.h"

/*
* 引擎API修改、扩展相关
*/

UCLASS()
class EXEDITORTOOLS_API UExEditorEngineLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/*
	* 对AssetViewUtils::DeleteFolders的扩展
	* 增加bShowConfirmation参数，实现删除文件夹时不弹确认框
	*/
	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
		static bool DeleteFolders(const TArray<FString>& PathsToDelete, bool bUseForceDelete=false, bool bShowConfirmation=true);

	/*
	*  拷贝自私有函数 AssetViewUtils::DeleteEmptyFolderFromDisk
	* 删除空文件夹，DeleteFolders用到的工具函数
	*/
	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
		static bool DeleteEmptyFolderFromDisk(const FString& InPathToDelete);

	/*
	* 拷贝自私有函数 AssetViewUtils::LoadAssetsIfNeeded
	* 加载资源，DeleteFolders用到的工具函数
	*/
	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
		static bool LoadAssetsIfNeeded(const TArray<FString>& ObjectPaths, TArray<UObject*>& LoadedObjects, bool bAllowedToPromptToLoadAssets, bool bLoadRedirects, bool bLoadWorldPartitionWorlds, bool bLoadAllExternalObjects);

	/*
	* 扩展自 FWorldTileCollectionModel::GenerateLODLevels
	* 原函数未导出，不能访问
	*/
	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
		static bool GenerateLODLevels(UWorld* MainWorld, TArray<ULevel*> InSubLevelList, int32 TargetLODIndex);

	/*
	* 扩展自  ObjectTools::DeleteObjects
	* 增加是否强制删除标志位，用于删除一些带有引用的资源
	* 去除是否展示对话框，默认不展示
	*/
	static int DeleteObjects(const TArray< UObject* >& InObjectsToDelete, bool bUseForceDelete = false, bool bAllowCancel = false);

	/*
	* 扩展自  ObjectTools::ContainsWorldInUse
	* 属于cpp中定义的私有函数，不能外部访问
	*/
	static bool ContainsWorldInUse(const TArray< UObject* >& ObjectsToDelete);

	/*
	* 扩展自  ObjectTools::HandleFullyLoadingPackages
	* dll未导出，不能使用
	*/
	static bool HandleFullyLoadingPackages(const TArray<UObject*>& Objects, const FText& OperationText);
};