#include "EditorUtils/ExEditorToolsLibrary.h"
#include "EditorUtils/ExEditorEngineLibrary.h"


#include "ContentBrowserAssetDataCore.h"
#include "ContentBrowserAssetDataSource.h"
#include "EditorLevelUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "FileHelpers.h"
#include "InstancedFoliageActor.h"



#define LOCTEXT_NAMESPACE "ExEditorToolsLibrary"

//蓝图相关工具函数
#pragma region Blueprint Tools

void UExEditorToolsLibrary::MarkBlueprintAsStructurallyModified(UBlueprint* Blueprint)
{
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
}

USCS_Node* UExEditorToolsLibrary::GetRootNodeByTag(UBlueprint* Blueprint, FName Tag)
{
	if (Blueprint == nullptr || Tag.IsValid() == false)
	{
		return nullptr;
	}

	const TArray<USCS_Node*>& RootNodes = Blueprint->SimpleConstructionScript->GetRootNodes();
	for (int i = 0; i < RootNodes.Num(); i++)
	{
		TObjectPtr<UActorComponent> RootComponent = Cast<UActorComponent>(RootNodes[i]->ComponentTemplate);
		if (RootComponent && RootComponent->ComponentHasTag(Tag))
		{
			return RootNodes[i];
		}
	}

	return nullptr;
}

TArray<UActorComponent*> UExEditorToolsLibrary::GetNodeChildComponentsByTag(USCS_Node* Node, FName Tag)
{
	TArray<UActorComponent*> Result;

	const TArray<USCS_Node*>& ChildNodes = Node->GetChildNodes();
	for (int i = 0; i < ChildNodes.Num(); i++)
	{
		UActorComponent* Component = ChildNodes[i]->ComponentTemplate;
		if (Component != nullptr && Component->ComponentHasTag(Tag))
		{
			Result.Add(Component);
		}
	}

	return Result;
}

void UExEditorToolsLibrary::ReparentBlueprint(UBlueprint* Blueprint, UClass* NewParentClass)
{
	if (!Blueprint || !NewParentClass)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("Failed to reparent blueprint!"));
		return;
	}

	if (NewParentClass == Blueprint->ParentClass)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("'%s' is already parented to class '%s'!"), *Blueprint->GetFriendlyName(), *NewParentClass->GetName());
		return;
	}

	if (!Blueprint->ParentClass || !NewParentClass->GetDefaultObject()->IsA(Blueprint->ParentClass))
	{
		EXEDITORTOOLS_LOG(Warning, TEXT("'%s' class heirarcy is changing, there could be possible data loss!"), *Blueprint->GetFriendlyName());
	}

	UClass* OriginalParentClass = Blueprint->ParentClass;
	Blueprint->ParentClass = NewParentClass;

	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	CompileBlueprint(Blueprint);
}

void UExEditorToolsLibrary::CompileBlueprint(UBlueprint* Blueprint)
{
	if (Blueprint)
	{
		// Skip saving this to avoid possible tautologies when saving and allow the user to manually save
		EBlueprintCompileOptions Flags = EBlueprintCompileOptions::SkipSave;
		FKismetEditorUtilities::CompileBlueprint(Blueprint, Flags);
	}
}
#pragma endregion

////浏览器资源操作相关工具函数
#pragma region Content Browser Tools

UContentBrowserAssetDataSource* UExEditorToolsLibrary::GetAssetDataSource()
{
	static UContentBrowserAssetDataSource* AssetDataSource = nullptr;

	if (AssetDataSource == nullptr)
	{
		FName ModularName = UContentBrowserDataSource::GetModularFeatureTypeName();
		TArray<UContentBrowserDataSource*> Browsers = IModularFeatures::Get().GetModularFeatureImplementations<UContentBrowserDataSource>(ModularName);

		for (int i = 0; i < Browsers.Num(); i++)
		{
			if (Browsers[i]->IsA(UContentBrowserAssetDataSource::StaticClass()))
			{
				AssetDataSource = Cast<UContentBrowserAssetDataSource>(Browsers[i]);
				break;
			}
		}
	}

	return AssetDataSource;
}

#define INIT_ASSET_DATA_SOURCE(FailedReturn) \
	UContentBrowserAssetDataSource* AssetDataSource = UExEditorToolsLibrary::GetAssetDataSource(); \
	if (AssetDataSource == nullptr) \
	{ \
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, Cannot get AssetDataSource"), *FString(__FUNCTION__));\
		return FailedReturn;\
	} \

FContentBrowserItemData UExEditorToolsLibrary::CreateAssetFileItem(const FName& AssetPath)
{
	INIT_ASSET_DATA_SOURCE(FContentBrowserItemData());

	UObject* AssetObject = StaticLoadObject(UObject::StaticClass(), nullptr, *AssetPath.ToString());
	if (AssetObject == nullptr)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("UExEditorToolsLibrary::CreateContentBrowserItemData error, cannot load object: %s"), *AssetPath.ToString());
		return FContentBrowserItemData();
	}

	if (!AssetObject->IsAsset() || !ContentBrowserAssetData::IsPrimaryAsset(AssetObject))
	{
		EXEDITORTOOLS_LOG(Error, TEXT("UExEditorToolsLibrary::CreateContentBrowserItemData error, Asset[%s] formate error"), *AssetPath.ToString());
		return FContentBrowserItemData();
	}

	FAssetData AssetData(AssetObject);
	return ContentBrowserAssetData::CreateAssetFileItem(AssetDataSource, AssetPath, AssetData);
}



FName UExEditorToolsLibrary::ConvertToVirtualPath(const FName& InternalPath)
{
	INIT_ASSET_DATA_SOURCE(InternalPath);

	FName VirtualPath;
	if (!AssetDataSource->TryConvertInternalPathToVirtual(InternalPath, VirtualPath))
	{
		return InternalPath;
	}
	return VirtualPath;
}

FName UExEditorToolsLibrary::ConvertToInternalPath(const FName& VirtualPath)
{
	INIT_ASSET_DATA_SOURCE(VirtualPath);

	FName InternalPath;
	if (!AssetDataSource->TryConvertInternalPathToVirtual(VirtualPath, InternalPath))
	{
		return VirtualPath;
	}
	return InternalPath;
}

#define CREAT_ASSET_FILE_ITEM(AssetPath) \
	INIT_ASSET_DATA_SOURCE(false); \
	FName VirtualAssetPath = UExEditorToolsLibrary::ConvertToVirtualPath(AssetPath);\
	FContentBrowserItemData AssetFileItem = UExEditorToolsLibrary::CreateAssetFileItem(AssetPath);\
	if (!AssetFileItem.IsValid()) \
	{ \
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, Cannot Get ContentBrowserItemData with path[%s]"), *FString(__FUNCTION__), *VirtualAssetPath.ToString()); \
		return false; \
	} \


#define CREAT_ASSET_FILE_ITEMS(AssetPaths) \
	INIT_ASSET_DATA_SOURCE(false); \
	TArray<FContentBrowserItemData> AssetFileItems; \
	for (const FName& AssetPath: AssetPaths) \
	{ \
		FContentBrowserItemData AssetFileItem = UExEditorToolsLibrary::CreateAssetFileItem(AssetPath);\
		if (!AssetFileItem.IsValid()) \
		{ \
			EXEDITORTOOLS_LOG(Error, TEXT("%s error, Cannot Get ContentBrowserItemData with path[%s]"), *FString(__FUNCTION__), *AssetPath.ToString()); \
			return false; \
		} \
		AssetFileItems.Add(AssetFileItem); \
	} \
	

bool UExEditorToolsLibrary::MoveAsset(const FName& AssetPath, const FName& DestPath)
{
	CREAT_ASSET_FILE_ITEM(AssetPath);

	FName VirtualDestPath = UExEditorToolsLibrary::ConvertToVirtualPath(DestPath);

	FText ErrMsg;
	if (AssetDataSource->CanMoveItem(AssetFileItem, VirtualDestPath, &ErrMsg) == false)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("UExEditorToolsLibrary::MoveAsset error, Cannot Move Asset[%s]: %s"), *AssetFileItem.GetInternalPath().ToString(), *ErrMsg.ToString());
		return false;
	}

	return AssetDataSource->MoveItem(AssetFileItem, VirtualDestPath);
}

bool UExEditorToolsLibrary::CopyAsset(const FName& AssetPath, const FName& DestPath)
{
	CREAT_ASSET_FILE_ITEM(AssetPath);
	FName VirtualDestPath = UExEditorToolsLibrary::ConvertToVirtualPath(DestPath);

	FText ErrMsg;
	if (AssetDataSource->CanCopyItem(AssetFileItem, VirtualDestPath, &ErrMsg) == false)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("UExEditorToolsLibrary::CopyAsset error, Cannot Move Asset[%s]: %s"), *AssetFileItem.GetInternalPath().ToString(), *ErrMsg.ToString());
		return false;
	}

	if (!AssetDataSource->CopyItem(AssetFileItem, VirtualDestPath))
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, AssetDataSource.CopyItem error"), *FString(__FUNCTION__));
		return false;
	}

	//保存
	FString AssetPackagePath = FPackageName::ObjectPathToPackageName(AssetPath.ToString());
	FString AssetName = FPackageName::GetShortName(AssetPackagePath);
	FString DestAssetPath = FString::Printf(TEXT("%s/%s.%s"), *DestPath.ToString(), *AssetName, *AssetName);
	if (!SaveAsset(FName(DestAssetPath), EContentBrowserItemSaveFlags::SaveOnlyIfLoaded))
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, Save Asset[%s] error"), *FString(__FUNCTION__), *DestAssetPath);
	}

	return true;
}

bool UExEditorToolsLibrary::DeleteAsset(const FName& AssetPath)
{
	CREAT_ASSET_FILE_ITEM(AssetPath);

	FText ErrMsg;
	if (AssetDataSource->CanDeleteItem(AssetFileItem,  &ErrMsg) == false)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("UExEditorToolsLibrary::DeleteAsset error, Cannot Delete Asset[%s]: %s"), *AssetFileItem.GetInternalPath().ToString(), *ErrMsg.ToString());
		return false;
	}

	return AssetDataSource->DeleteItem(AssetFileItem);

}

bool UExEditorToolsLibrary::SaveAsset(const FName& AssetPath, EContentBrowserItemSaveFlags InSaveFlags)
{
	CREAT_ASSET_FILE_ITEM(AssetPath);

	FText ErrMsg;
	if (AssetDataSource->CanSaveItem(AssetFileItem, InSaveFlags, &ErrMsg) == false)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, Cannot Save Asset[%s]: %s"), *FString(__FUNCTION__), *AssetFileItem.GetInternalPath().ToString(), *ErrMsg.ToString());
		return false;
	}

	return AssetDataSource->SaveItem(AssetFileItem, InSaveFlags);
}

bool UExEditorToolsLibrary::SaveAssets(const TArray<FName>& AssetPaths, EContentBrowserItemSaveFlags InSaveFlags)
{
	CREAT_ASSET_FILE_ITEMS(AssetPaths);
	return AssetDataSource->BulkSaveItems(AssetFileItems, InSaveFlags);
}

bool UExEditorToolsLibrary::CreateFolder(const FName& FolderPath)
{
	INIT_ASSET_DATA_SOURCE(false);

	const FString FolderItemName = FPackageName::GetShortName(FolderPath);
	FName VirtualFolderPath = UExEditorToolsLibrary::ConvertToVirtualPath(FolderPath);

	FText ErrMsg;
	if (AssetDataSource->CanCreateFolder(VirtualFolderPath, &ErrMsg) == false)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, Cannot Create Folder[%s]"), *FString(__FUNCTION__), *VirtualFolderPath.ToString());
		return false;
	}

 	FContentBrowserItemDataTemporaryContext OutPendingItem;
 	bool Result = AssetDataSource->CreateFolder(VirtualFolderPath, OutPendingItem);
	if (Result && OutPendingItem.IsValid())
	{
		OutPendingItem.FinalizeItem(FolderItemName, &ErrMsg);
		EXEDITORTOOLS_LOG(Log, TEXT("%s success, Folder[%s]"), *FString(__FUNCTION__), *FolderPath.ToString());
		return true;
	}

	EXEDITORTOOLS_LOG(Log, TEXT("%s error, Create Folder[%s] error"), *FString(__FUNCTION__), *VirtualFolderPath.ToString());
	return false;
}

bool UExEditorToolsLibrary::DeleteFolder(const FName& InPath)
{
	INIT_ASSET_DATA_SOURCE(false);

	FName VirualPath = UExEditorToolsLibrary::ConvertToVirtualPath(InPath);

	FContentBrowserItemData AssetFolderItem = ContentBrowserAssetData::CreateAssetFolderItem(AssetDataSource, VirualPath, InPath);
	FText ErrMsg;
	if (AssetDataSource->CanDeleteItem(AssetFolderItem, &ErrMsg) == false)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("UExEditorToolsLibrary::DeleteAsset error, Cannot Delete Folder[%s]: %s"), *AssetFolderItem.GetInternalPath().ToString(), *ErrMsg.ToString());
		return false;
	}

	return AssetDataSource->DeleteItem(AssetFolderItem);

}



#pragma endregion

//关卡操作相关工具函数
#pragma region World & Level Tools

#pragma endregion

#undef LOCTEXT_NAMESPACE