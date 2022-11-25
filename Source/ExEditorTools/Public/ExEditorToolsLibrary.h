#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "ExEditorToolsLibrary.generated.h"

UCLASS()
class EXEDITORTOOLS_API UExEditorToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void MarkBlueprintAsStructurallyModified(UBlueprint* Blueprint);

	static UWidget* ClearWidget(UWidgetTree* WidgetTree, FString RootName);

	static USCS_Node* GetRootNodeByTag(UBlueprint* Blueprint, FName Tag);

	static TArray<UActorComponent*> GetNodeChildComponentsByTag(USCS_Node* Node, FName Tag);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static void ReparentBlueprint(UBlueprint* Blueprint, UClass* NewParentClass);

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static void CompileBlueprint(UBlueprint* Blueprint);

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

#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "External Editor Tools")
	static bool CopyWorldActors(TSoftObjectPtr<UWorld> SrcWorld, TSoftObjectPtr<UWorld> DestWorld);
	
};