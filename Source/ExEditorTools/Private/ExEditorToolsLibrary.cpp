#include "ExEditorToolsLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "ExEditorToolsModule.h"
#include "ContentBrowserAssetDataCore.h"
#include "ContentBrowserAssetDataSource.h"
#include "EditorLevelUtils.h"
#include "Factories/WorldFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "FileHelpers.h"
#include "InstancedFoliageActor.h"
#include "ExEditorEngineLibrary.h"
#include "Engine/LevelBounds.h"


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
bool UExEditorToolsLibrary::CopyWorldActors(const FString& SrcWorldPath, const FString& DestWorldPath)
{
	UWorld* SrcWorld = nullptr;

	UPackage* SrcLevelPackage = LoadPackage(NULL, *SrcWorldPath, LOAD_EditorOnly);
	if (SrcLevelPackage)
	{
		SrcWorld = UWorld::FindWorldInPackage(SrcLevelPackage);
	}
	if (!SrcWorld)
	{
		return false;
	}

	FString PathName = SrcWorld->GetPathName();
	FString ObjectName = FPackageName::ObjectPathToObjectName(PathName);
	FString LongPackageName = FPackageName::GetLongPackagePath(PathName);

	UWorld* DestWorld = CreateWorld(DestWorldPath, "LevelC");
	if (!DestWorld)
	{
		return false;
	}
	SaveAsset(FName(DestWorldPath), EContentBrowserItemSaveFlags::SaveOnlyIfLoaded);


	ULevel* DestLevel = DestWorld->PersistentLevel;
	ULevel* SrcLevel = SrcWorld->PersistentLevel;
	TArray<AActor*> SrcActors = SrcWorld->PersistentLevel->Actors;

//	UEditorLevelUtils::MakeLevelCurrent(DestLevel);
	UExEditorEngineLibrary::CopyOrMoveActorsToLevel(SrcLevel, SrcActors, DestLevel, false);

	FString DestWorldPathName = DestLevel->OwningWorld->GetPathName();
	SaveAsset(FName(DestWorldPathName), EContentBrowserItemSaveFlags::SaveOnlyIfLoaded);
	SaveAsset(FName(SrcWorld->GetPathName()), EContentBrowserItemSaveFlags::SaveOnlyIfLoaded);

	//ULevel* CurrentEditorLevel = GEditor->GetEditorWorldContext().World()->PersistentLevel;
	//UEditorLevelUtils::MakeLevelCurrent(CurrentEditorLevel);
	UEditorLevelUtils::MakeLevelCurrent(SrcWorld->PersistentLevel);

	TArray<UPackage*> LoadedPackage;
	LoadedPackage.Add(DestWorld->GetPackage());
	FText ErrorMsg;
	UPackageTools::UnloadPackages(LoadedPackage, ErrorMsg);

	return true;
}

UWorld* UExEditorToolsLibrary::CreateWorld(const FString WorldPath, const FString WorldName)
{
	UPackage* Package = CreatePackage(*FString::Printf(TEXT("%s/%s"), *WorldPath, *WorldName));

	// 使用工厂类初始化 UWorld
	UWorldFactory* WorldFactory = NewObject<UWorldFactory>();
	UObject* ObjectWorld = WorldFactory->FactoryCreateNew(WorldFactory->SupportedClass, Package, *WorldName, EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, nullptr, GWarn);
	UWorld* World = Cast<UWorld>(ObjectWorld);

	// 创建 UWorld
	FAssetRegistryModule::AssetCreated(ObjectWorld);

	//创建LevelBounds
	ALevelBounds* LevelBoundsActor =	Cast<ALevelBounds>(World->SpawnActor(ALevelBounds::StaticClass()));
	World->PersistentLevel->LevelBoundsActor = LevelBoundsActor;

	// Save
	if (World->MarkPackageDirty())
	{
		EXEDITORTOOLS_LOG(Warning, TEXT("[[ TiledWorld ]] << bIsDirtyMarked >> Successed "));
		FEditorFileUtils::SaveDirtyPackages(false, true, true);
	}
	else
	{
		EXEDITORTOOLS_LOG(Error, TEXT("[[ TiledWorld ]] << bIsDirtyMarked >> Suppressed by the editor"));
	}
	return World;
}

UWorld* UExEditorToolsLibrary::LoadWorldFromPath(const FString& PackagePath)
{
	UPackage* LevelPackage = LoadPackage(NULL, *PackagePath, LOAD_EditorOnly);
	if (LevelPackage)
	{
		return UWorld::FindWorldInPackage(LevelPackage);
	}
	return nullptr;
}

TMap<FName, FWorldCompositionTile*> UExEditorToolsLibrary::GetWorldCompsitionTile(UWorld* World)
{
	TMap<FName, FWorldCompositionTile*> Result;
	if (!World || !World->WorldComposition)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, World Must Be World Composition"), *FString(__FUNCTION__));
		return Result;
	}

	UWorldComposition::FTilesList& TilesList = World->WorldComposition->GetTilesList();
	for (FWorldCompositionTile& Tile : TilesList)
	{
		Result.Add(Tile.PackageName, &Tile);
	}

	return Result;
}

TArray<ULevel*> UExEditorToolsLibrary::FindSubLevel(UWorld* World, TArray<FName> LevelPackageName)
{
	TArray<ULevel*> SubLevels;

	if (!World || !World->WorldComposition)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, World Must Be World Composition"), *FString(__FUNCTION__));
		return SubLevels;
	}

	UWorldComposition::FTilesList& TilesList = World->WorldComposition->GetTilesList();
	for (FWorldCompositionTile& Tile : TilesList)
	{
		if (LevelPackageName.Contains(Tile.PackageName))
		{
			UWorld* SubLevelWorld = LoadWorldFromPath(Tile.PackageName.ToString());
			if (SubLevelWorld)
			{
				SubLevels.Add(SubLevelWorld->PersistentLevel);
			}
		}
	}

	return SubLevels;
}

bool UExEditorToolsLibrary::UpdateLevelTileInfo(UWorld* World, TArray<FName> LevelPackageNames, FBPTiledWorldInfo TileInfo)
{
	if (!World || !World->WorldComposition)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, World Must Be World Composition"), *FString(__FUNCTION__));
		return false;
	}

	TMap<FName, FWorldCompositionTile*> TileMap = UExEditorToolsLibrary::GetWorldCompsitionTile(World);

	FScopedSlowTask UpdateProgress(LevelPackageNames.Num(), LOCTEXT("UpdateLevelTileInfo", "Update Level TileInfo"));
	UpdateProgress.MakeDialog();

	for (const FName& PackageName: LevelPackageNames)
	{
		UpdateProgress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Update %s"), *PackageName.ToString())));
		if (!TileMap.Contains(PackageName))
		{
			continue;
		}

		FWorldCompositionTile* Tile = TileMap[PackageName];
		UWorld* SubLevelWorld = LoadWorldFromPath(Tile->PackageName.ToString());
		if (SubLevelWorld)
		{
			FWorldTileInfo NewTileInfo = Tile->Info;

			if (TileInfo.Layer.IsValid())
			{
				NewTileInfo.Layer.Name = TileInfo.Layer.Name;
				NewTileInfo.Layer.StreamingDistance = TileInfo.Layer.StreamingDistance;
				NewTileInfo.Layer.DistanceStreamingEnabled = true;
				NewTileInfo.ParentTilePackageName = FName(NAME_None).ToString();

				if (SubLevelWorld->PersistentLevel->LevelBoundsActor.IsValid())
				{
					NewTileInfo.Bounds = SubLevelWorld->PersistentLevel->LevelBoundsActor->GetComponentsBoundingBox();
				}
			}

			NewTileInfo.LODList.Reset();
			for (auto& LODItem : TileInfo.LODList)
			{
				if (LODItem.IsValid())
				{
					FWorldTileLODInfo* LODInfo = new (NewTileInfo.LODList)FWorldTileLODInfo();
					LODInfo->RelativeStreamingDistance = LODItem.RelativeStreamingDistance;
				}
			}

			World->WorldComposition->OnTileInfoUpdated(Tile->PackageName, NewTileInfo);
		}
	}

	return true;
}
#pragma endregion

#undef LOCTEXT_NAMESPACE