#include "ExEditorToolsLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "ExEditorToolsModule.h"
#include "ContentBrowserAssetDataCore.h"
#include "ContentBrowserAssetDataSource.h"
#include "EditorLevelUtils.h"

void UExEditorToolsLibrary::MarkBlueprintAsStructurallyModified(UBlueprint* Blueprint)
{
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
}

UWidget* UExEditorToolsLibrary::ClearWidget(UWidgetTree* WidgetTree, FString RootName)
{
	if (RootName.IsEmpty()|| !WidgetTree)
	{
		return nullptr;
	}

	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);

	UWidget* RootWidget = nullptr;

	for (int i = 0; i < Widgets.Num(); i++)
	{
		UWidget* Widget = Widgets[i];

		if (RootWidget == nullptr && Widget->GetName() == RootName)
		{
			RootWidget = Widget;
			continue;
		}

		if (!Widget->Slot || !Widget->Slot->Parent)
		{
			continue;
		}

		TObjectPtr<UWidget> Parent = Widget->Slot->Parent;
		if (Parent->GetName() == RootName)
		{
			WidgetTree->RemoveWidget(Widget);
		}
	}

	return RootWidget;
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

	return AssetDataSource->CopyItem(AssetFileItem, VirtualDestPath);
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


bool UExEditorToolsLibrary::CopyWorldActors(TSoftObjectPtr<UWorld> SrcWorld, TSoftObjectPtr<UWorld> DestWorld)
{
	if (SrcWorld.IsValid() && DestWorld.IsValid())
	{
		TArray<AActor*> SrcActors = SrcWorld.Get()->PersistentLevel->Actors;
		UEditorLevelUtils::CopyActorsToLevel(SrcActors, DestWorld.Get()->PersistentLevel);
		return true;
	}
	return false;
}
