#include "EditorUtils/ExEditorLevelLibrary.h"
#include "ExEditorToolTypes.h"

#define LOCTEXT_NAMESPACE "UExEditorLevelLibrary"

bool UExEditorLevelLibrary::CopyWorldActors(const FString& SrcWorldPath, const FString& DestWorldPath)
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
	UExEditorToolsLibrary::SaveAsset(FName(DestWorldPath), EContentBrowserItemSaveFlags::SaveOnlyIfLoaded);


	ULevel* DestLevel = DestWorld->PersistentLevel;
	ULevel* SrcLevel = SrcWorld->PersistentLevel;
	TArray<AActor*> SrcActors = SrcWorld->PersistentLevel->Actors;

	//	UEditorLevelUtils::MakeLevelCurrent(DestLevel);
	UExEditorEngineLibrary::CopyOrMoveActorsToLevel(SrcLevel, SrcActors, DestLevel, false);

	FString DestWorldPathName = DestLevel->OwningWorld->GetPathName();
	UExEditorToolsLibrary::SaveAsset(FName(DestWorldPathName), EContentBrowserItemSaveFlags::SaveOnlyIfLoaded);
	UExEditorToolsLibrary::SaveAsset(FName(SrcWorld->GetPathName()), EContentBrowserItemSaveFlags::SaveOnlyIfLoaded);

	//ULevel* CurrentEditorLevel = GEditor->GetEditorWorldContext().World()->PersistentLevel;
	//UEditorLevelUtils::MakeLevelCurrent(CurrentEditorLevel);
	UEditorLevelUtils::MakeLevelCurrent(SrcWorld->PersistentLevel);

	TArray<UPackage*> LoadedPackage;
	LoadedPackage.Add(DestWorld->GetPackage());
	FText ErrorMsg;
	UPackageTools::UnloadPackages(LoadedPackage, ErrorMsg);

	return true;
}

UWorld* UExEditorLevelLibrary::CreateWorld(const FString WorldPath, const FString WorldName)
{
	UPackage* Package = CreatePackage(*FString::Printf(TEXT("%s/%s"), *WorldPath, *WorldName));

	// 使用工厂类初始化 UWorld
	UWorldFactory* WorldFactory = NewObject<UWorldFactory>();
	UObject* ObjectWorld = WorldFactory->FactoryCreateNew(WorldFactory->SupportedClass, Package, *WorldName, EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, nullptr, GWarn);
	UWorld* World = Cast<UWorld>(ObjectWorld);

	// 创建 UWorld
	FAssetRegistryModule::AssetCreated(ObjectWorld);

	//创建LevelBounds
	ALevelBounds* LevelBoundsActor = Cast<ALevelBounds>(World->SpawnActor(ALevelBounds::StaticClass()));
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

UWorld* UExEditorLevelLibrary::LoadWorldFromPath(const FString& PackagePath)
{
	UPackage* LevelPackage = LoadPackage(NULL, *PackagePath, LOAD_EditorOnly);
	if (LevelPackage)
	{
		return UWorld::FindWorldInPackage(LevelPackage);
	}
	return nullptr;
}

TMap<FName, FWorldCompositionTile*> UExEditorLevelLibrary::GetWorldCompsitionTile(UWorld* World)
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

TArray<ULevel*> UExEditorLevelLibrary::FindSubLevel(UWorld* World, TArray<FName> LevelPackageName)
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

bool UExEditorLevelLibrary::UpdateLevelTileInfo(UWorld* World, TArray<FName> LevelPackageNames, FBPTiledWorldInfo TileInfo)
{
	if (!World || !World->WorldComposition)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, World Must Be World Composition"), *FString(__FUNCTION__));
		return false;
	}

	TMap<FName, FWorldCompositionTile*> TileMap = UExEditorLevelLibrary::GetWorldCompsitionTile(World);

	FScopedSlowTask UpdateProgress(LevelPackageNames.Num(), LOCTEXT("UpdateLevelTileInfo", "Update Level TileInfo"));
	UpdateProgress.MakeDialog();

	for (const FName& PackageName : LevelPackageNames)
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

bool UExEditorLevelLibrary::ReorderSubLevels(UWorld* World, TMap<FName, int> SubLevelPriorities)
{
	if (!World || !World->WorldComposition)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, World Must Be World Composition"), *FString(__FUNCTION__));
		return false;
	}

	//需要重排序的数据
	UWorldComposition::FTilesList& TilesList = World->WorldComposition->GetTilesList();

	TilesList.Sort([&](const FWorldCompositionTile& A, const FWorldCompositionTile& B) 
	{
			FName ALevelName = FPackageName::GetShortFName(A.PackageName);
			FName BLevelName = FPackageName::GetShortFName(B.PackageName);

			int APriority = SubLevelPriorities.Contains(ALevelName) ? SubLevelPriorities[ALevelName] : 0;
			int BPriority = SubLevelPriorities.Contains(BLevelName) ? SubLevelPriorities[BLevelName] : 0;

			return APriority > BPriority;
	});

	UExEditorToolsLibrary::SaveAsset(World->GetPackage()->GetFName(), EContentBrowserItemSaveFlags::SaveOnlyIfLoaded);

	return true;
}

#undef LOCTEXT_NAMESPACE
