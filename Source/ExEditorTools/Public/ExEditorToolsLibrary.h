#pragma once

#include "CoreMinimal.h"
#include "ExTypes.h"
#include "Components/Widget.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "ContentBrowserAssetDataCore.h"
#include "ContentBrowserAssetDataSource.h"
#include "Engine/WorldComposition.h"
#include "EditorUtils/ExEditorFoliageLibrary.h"
#include "ExEditorToolsLibrary.generated.h"


UCLASS()
class EXEDITORTOOLS_API UExEditorToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	

//蓝图相关工具函数
#pragma region Blueprint Tools

	static void MarkBlueprintAsStructurallyModified(UBlueprint* Blueprint);

	static USCS_Node* GetRootNodeByTag(UBlueprint* Blueprint, FName Tag);

	static TArray<UActorComponent*> GetNodeChildComponentsByTag(USCS_Node* Node, FName Tag);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static void ReparentBlueprint(UBlueprint* Blueprint, UClass* NewParentClass);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static void CompileBlueprint(UBlueprint* Blueprint);

#pragma endregion

//浏览器资源操作相关工具函数
#pragma region Content Browser Tools

	static class UContentBrowserAssetDataSource* GetAssetDataSource();
	static class FContentBrowserItemData CreateAssetFileItem(const FName& AssetPath);

	UFUNCTION(BlueprintPure, Category = "External Editor Tools")
	static FName ConvertToVirtualPath(const FName& InternalPath);

	UFUNCTION(BlueprintPure, Category = "External Editor Tools")
	static FName ConvertToInternalPath(const FName& VirtualPath);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static bool MoveAsset(const FName& AssetPath, const FName& DestPath);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static bool CopyAsset(const FName& AssetPath, const FName& DestPath);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static bool DeleteAsset(const FName& AssetPath);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static bool CreateFolder(const FName& InPath);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static bool DeleteFolder(const FName& InPath);

	static bool SaveAsset(const FName& AssetPath, EContentBrowserItemSaveFlags InSaveFlags);

	static bool SaveAssets(const TArray<FName>& AssetPaths, EContentBrowserItemSaveFlags InSaveFlags);

#pragma endregion

//关卡操作相关工具函数
#pragma region World & Level Tools
	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static bool CopyWorldActors(const FString& SrcWorldPath, const FString& DescWorldPath);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static UWorld* CreateWorld(FString WorldPath, FString WorldName);
	
	UFUNCTION(BlueprintPure, Category="External Editor Tools")
	static UWorld* LoadWorldFromPath(const FString& PackagePath);

	static TMap<FName, FWorldCompositionTile*> GetWorldCompsitionTile(UWorld* World);

	UFUNCTION(BlueprintPure, Category = "External Editor Tools")
	static TArray<ULevel*> FindSubLevel(UWorld* World, TArray<FName> LevelPackageName);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static bool UpdateLevelTileInfo(UWorld* World, TArray<FName> LevelPackageName,  FBPTiledWorldInfo TileInfo);
#pragma endregion

//引擎API修改、扩展相关
#pragma region Engine API Extension

	
#pragma endregion
};