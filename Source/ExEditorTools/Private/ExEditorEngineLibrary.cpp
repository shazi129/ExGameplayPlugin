#include "ExEditorEngineLibrary.h"
#include "ExEditorToolsModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "LevelUtils.h"
#include "Engine/LevelStreaming.h"
#include "Engine/Selection.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Misc/Paths.h"
#include "Misc/NamePermissionList.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FeedbackContext.h"
#include "Misc/ScopedSlowTask.h"
#include "Misc/WorldCompositionUtility.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "EditorLevelUtils.h"
#include "IMeshReductionInterfaces.h"
#include "LandscapeProxy.h"
#include "LandscapeMeshProxyActor.h"
#include "LandscapeMeshProxyComponent.h"
#include "MeshMergeModule.h"
#include "MaterialUtilities.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "ExGameplayLibrary.h"
#include "InstancedFoliageActor.h"
#include "TerrainEditorUtils.h"
#include "StaticMeshAttributes.h"
#include "Engine/StaticMeshActor.h"
#include "FileHelpers.h"
#include "AssetViewUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ExEditorToolsLibrary.h"
#include "BusyCursor.h"
#include "AssetDeleteModel.h"
#include "Dialogs/SDeleteAssetsDialog.h"
#include "AudioDevice.h"
#include "PackageTools.h"

#define LOCTEXT_NAMESPACE "ExEditorEngineLibrary"

int32 UExEditorEngineLibrary::CopyOrMoveActorsToLevel(
	ULevel* SrcLevel, const TArray<AActor*>& ActorsToMove, ULevel* DestLevel,
	bool bMoveActors, bool bWarnAboutReferences, bool bWarnAboutRenaming,
	bool bMoveAllOrFail, TArray<AActor*>* OutActors)
{
	int32 NumMovedActors = 0;

	if (SrcLevel && DestLevel)
	{
		UWorld* SrcOwningWorld = SrcLevel->OwningWorld;
		UWorld* DestOwningWorld = DestLevel->OwningWorld;

		// Backup the current contents of the clipboard string as we'll be using cut/paste features to move actors
		// between levels and this will trample over the clipboard data.
		FString OriginalClipboardContent;
		FPlatformApplicationMisc::ClipboardPaste(OriginalClipboardContent);

		// The final list of actors to move after invalid actors were removed
		TArray<AActor*> FinalMoveList;
		TArray<TWeakObjectPtr<AActor>> FinalWeakMoveList;
		FinalMoveList.Reserve(ActorsToMove.Num());

		bool bIsDestLevelLocked = FLevelUtils::IsLevelLocked(DestLevel);
		bool bIsSrcLevelLocked = FLevelUtils::IsLevelLocked(SrcLevel);

		int32 ActorCount = 0;
		if (!bIsDestLevelLocked && !bIsSrcLevelLocked)
		{
			for (AActor* CurActor : ActorsToMove)
			{
				//AInstancedFoliageActor* TargetIFA = AInstancedFoliageActor::GetInstancedFoliageActorForLevel(SrcLevel, false);
				if (!CurActor)
				{
					continue;
				}
				ActorCount++;
				bool bIsSourceLevelLocked = FLevelUtils::IsLevelLocked(CurActor);

				if (!bIsSourceLevelLocked)
				{
					if (CurActor->GetLevel() != DestLevel)
					{
						bool bCanMove = true;
						UEditorLevelUtils::CanMoveActorToLevelDelegate.Broadcast(CurActor, DestLevel, bCanMove);
						if (bCanMove)
						{
							FinalMoveList.Add(CurActor);
						}
					}
					else
					{
						EXEDITORTOOLS_LOG(Warning, TEXT("%s is already in the destination level so it was ignored"), *CurActor->GetName());
					}
				}
				else
				{
					EXEDITORTOOLS_LOG(Error, TEXT("The source level '%s' is locked so actors could not be moved"), *CurActor->GetLevel()->GetName());
				}
			}
		}
		else
		{
			EXEDITORTOOLS_LOG(Error, TEXT("The destination level '%s' is locked so actors could not be moved"), *DestLevel->GetName());
		}


		if (FinalMoveList.Num() > 0 && (!bMoveAllOrFail || FinalMoveList.Num() == ActorCount))
		{
			TArray<TTuple<FSoftObjectPath, FSoftObjectPath>> ActorPathMapping;
			GEditor->SelectNone(false, true, false);

			USelection* ActorSelection = GEditor->GetSelectedActors();
			ActorSelection->BeginBatchSelectOperation();
			FinalWeakMoveList.Reserve(FinalMoveList.Num());
			for (AActor* Actor : FinalMoveList)
			{
				FinalWeakMoveList.Add(Actor);
				check(Actor->CopyPasteId == INDEX_NONE);
				Actor->CopyPasteId = ActorPathMapping.Add(TTuple<FSoftObjectPath, FSoftObjectPath>(FSoftObjectPath(Actor), FSoftObjectPath()));
				GEditor->SelectActor(Actor, true, false);
			}
			ActorSelection->EndBatchSelectOperation(false);

			if (GEditor->GetSelectedActorCount() > 0)
			{
				// Start the transaction
				FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "MoveSelectedActorsToSelectedLevel", "Move Actors To Level"));

				// Broadcast event
				if (bMoveActors)
				{
					UEditorLevelUtils::OnMoveActorsToLevelEvent.Broadcast(ActorsToMove, DestLevel);
				}

				// Cache the old level
				//ULevel* OldCurrentLevel = SrcOwningWorld->GetCurrentLevel();

				FString DestinationData;
				GEditor->CopySelectedActorsToClipboard(SrcOwningWorld, bMoveActors, bMoveActors, bWarnAboutReferences, &DestinationData);

				const bool bLevelVisible = DestLevel->bIsVisible;
				if (!bLevelVisible)
				{
					UEditorLevelUtils::SetLevelVisibility(DestLevel, true, false);
				}

				// Scope this so that Actors that have been pasted will have their final levels set before doing the actor mapping
				{
					// Set the new level and force it visible while we do the paste
					FLevelPartitionOperationScope LevelPartitionScope(DestLevel);
					DestOwningWorld->SetCurrentLevel(LevelPartitionScope.GetLevel());

					const bool bDuplicate = false;
					const bool bOffsetLocations = false;
					const bool bWarnIfHidden = false;
					GEditor->edactPasteSelected(DestOwningWorld, bDuplicate, bOffsetLocations, bWarnIfHidden, &DestinationData);

					// Restore the original current level
					SrcOwningWorld->SetCurrentLevel(SrcLevel);
				}

				// Build a remapping of old to new names so we can do a fixup
				for (FSelectionIterator It(GEditor->GetSelectedActorIterator()); It; ++It)
				{
					AActor* Actor = static_cast<AActor*>(*It);

					if (ActorPathMapping.IsValidIndex(Actor->CopyPasteId))
					{
						TTuple<FSoftObjectPath, FSoftObjectPath>& Tuple = ActorPathMapping[Actor->CopyPasteId];
						check(Tuple.Value.IsNull());

						Tuple.Value = FSoftObjectPath(Actor);
						if (OutActors)
						{
							OutActors->Add(Actor);
						}
					}
					else
					{
						EXEDITORTOOLS_LOG(Error, TEXT("Cannot find remapping for moved actor ID %s, any soft references pointing to it will be broken!"), *Actor->GetPathName());
					}
					// Reset CopyPasteId on new actors
					Actor->CopyPasteId = INDEX_NONE;
				}

				if (SrcOwningWorld == DestOwningWorld)
				{
					FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
					TArray<FAssetRenameData> RenameData;

					for (TTuple<FSoftObjectPath, FSoftObjectPath>& Pair : ActorPathMapping)
					{
						if (Pair.Value.IsValid())
						{
							RenameData.Add(FAssetRenameData(Pair.Key, Pair.Value, true));
						}
					}

					if (RenameData.Num() > 0)
					{
						if (bWarnAboutRenaming)
						{
							AssetToolsModule.Get().RenameAssetsWithDialog(RenameData);
						}
						else
						{
							AssetToolsModule.Get().RenameAssets(RenameData);
						}
					}
				}

				// Restore new level visibility to previous state
				if (!bLevelVisible)
				{
					UEditorLevelUtils::SetLevelVisibility(DestLevel, false, false);
				}
			}

			// The moved (pasted) actors will now be selected
			NumMovedActors += FinalMoveList.Num();

			for (TWeakObjectPtr<AActor> ActorPtr : FinalWeakMoveList)
			{
				// It is possible a GC happens because of RenameAssets being called so here we want to update the CopyPasteId only on reachable actors
				if (AActor* Actor = ActorPtr.Get(/*bEvenIfPendingKill=*/true))
				{
					check(SrcOwningWorld != DestOwningWorld || Actor->CopyPasteId != INDEX_NONE);
					// Reset CopyPasteId on source actors 
					Actor->CopyPasteId = INDEX_NONE;
				}
			}
		}

		// Restore the original clipboard contents
		FPlatformApplicationMisc::ClipboardCopy(*OriginalClipboardContent);
	}

	return NumMovedActors;
}

bool UExEditorEngineLibrary::LoadAssetsIfNeeded(const TArray<FString>& ObjectPaths, TArray<UObject*>& LoadedObjects, bool bAllowedToPromptToLoadAssets, bool bLoadRedirects, bool bLoadWorldPartitionWorlds, bool bLoadAllExternalObjects)
{
	bool bAnyObjectsWereLoadedOrUpdated = false;

	// Build a list of unloaded assets
	TArray<FString> UnloadedObjectPaths;
	bool bAtLeastOneUnloadedMap = false;
	for (int32 PathIdx = 0; PathIdx < ObjectPaths.Num(); ++PathIdx)
	{
		const FString& ObjectPath = ObjectPaths[PathIdx];

		UObject* FoundObject = FindObject<UObject>(NULL, *ObjectPath);
		if (FoundObject)
		{
			LoadedObjects.Add(FoundObject);
		}
		else
		{
			if (FEditorFileUtils::IsMapPackageAsset(ObjectPath))
			{
				FName PackageName = FName(*FEditorFileUtils::ExtractPackageName(ObjectPath));
				if (!bLoadWorldPartitionWorlds && ULevel::GetIsLevelPartitionedFromPackage(PackageName))
				{
					continue;
				}

				bAtLeastOneUnloadedMap = true;
			}

			// Unloaded asset, we will load it later
			UnloadedObjectPaths.Add(ObjectPath);
		}
	}

	// Make sure all selected objects are loaded, where possible
	if (UnloadedObjectPaths.Num() > 0)
	{
		// Get the maximum objects to load before displaying the slow task
		const bool bShowProgressDialog = (UnloadedObjectPaths.Num() > GetDefault<UContentBrowserSettings>()->NumObjectsToLoadBeforeWarning) || bAtLeastOneUnloadedMap;
		FScopedSlowTask SlowTask(UnloadedObjectPaths.Num(), LOCTEXT("LoadingObjects", "Loading Objects..."));
		if (bShowProgressDialog)
		{
			SlowTask.MakeDialog();
		}

		GIsEditorLoadingPackage = true;

		// We usually don't want to follow redirects when loading objects for the Content Browser.  It would
		// allow a user to interact with a ghost/unverified asset as if it were still alive.
		// This can be overridden by providing bLoadRedirects = true as a parameter.
		const ELoadFlags LoadFlags = bLoadRedirects ? LOAD_None : LOAD_NoRedirects;

		bool bSomeObjectsFailedToLoad = false;
		for (int32 PathIdx = 0; PathIdx < UnloadedObjectPaths.Num(); ++PathIdx)
		{
			const FString& ObjectPath = UnloadedObjectPaths[PathIdx];
			SlowTask.EnterProgressFrame(1, FText::Format(LOCTEXT("LoadingObjectf", "Loading {0}..."), FText::FromString(ObjectPath)));

			// Load up the object
			TUniquePtr<FScopedLoadAllExternalObjects> Scope(bLoadAllExternalObjects ? new FScopedLoadAllExternalObjects(*FEditorFileUtils::ExtractPackageName(ObjectPath)) : nullptr);
			UObject* LoadedObject = LoadObject<UObject>(NULL, *ObjectPath, NULL, LoadFlags, NULL);
			if (LoadedObject)
			{
				LoadedObjects.Add(LoadedObject);
			}
			else
			{
				bSomeObjectsFailedToLoad = true;
			}

			if (GWarn->ReceivedUserCancel())
			{
				// If the user has canceled stop loading the remaining objects. We don't add the remaining objects to the failed string,
				// this would only result in launching another dialog when by their actions the user clearly knows not all of the 
				// assets will have been loaded.
				break;
			}
		}
		GIsEditorLoadingPackage = false;

		if (bSomeObjectsFailedToLoad)
		{
			FNotificationInfo Info(LOCTEXT("LoadObjectFailed", "Failed to load assets"));
			Info.ExpireDuration = 5.0f;
			Info.Hyperlink = FSimpleDelegate::CreateStatic([]() { FMessageLog("LoadErrors").Open(EMessageSeverity::Info, true); });
			Info.HyperlinkText = LOCTEXT("LoadObjectHyperlink", "Show Message Log");

			FSlateNotificationManager::Get().AddNotification(Info);
			return false;
		}
	}

	return true;
}

bool UExEditorEngineLibrary::DeleteFolders(const TArray<FString>& PathsToDelete, bool bUseForceDelete, bool bShowConfirmation)
{
	// Get a list of assets in the paths to delete
	TArray<FAssetData> AssetDataList;
	AssetViewUtils::GetAssetsInPaths(PathsToDelete, AssetDataList);

	const int32 NumAssetsInPaths = AssetDataList.Num();
	bool bAllowFolderDelete = false;
	if (NumAssetsInPaths == 0)
	{
		// There were no assets, allow the folder delete.
		bAllowFolderDelete = true;
	}
	else
	{
		// Load all the assets in the folder and attempt to delete them.
		// If it was successful, allow the folder delete.

		// Get a list of object paths for input into LoadAssetsIfNeeded
		TArray<FString> ObjectPaths;
		for (auto AssetIt = AssetDataList.CreateConstIterator(); AssetIt; ++AssetIt)
		{
			ObjectPaths.Add((*AssetIt).ObjectPath.ToString());
		}

		// Load all the assets in the selected paths
		TArray<UObject*> LoadedAssets;
		const bool bAllowedToPromptToLoadAssets = true;
		const bool bLoadRedirects = false;
		const bool bLoadWorldPartitionWorlds = true;
		const bool bLoadAllExternalObjects = true;
		if (UExEditorEngineLibrary::LoadAssetsIfNeeded(ObjectPaths, LoadedAssets, bAllowedToPromptToLoadAssets, bLoadRedirects, bLoadWorldPartitionWorlds, bLoadAllExternalObjects))
		{
			// Make sure we loaded all of them
			if (LoadedAssets.Num() == NumAssetsInPaths)
			{
				TArray<UObject*> ToDelete = LoadedAssets;
				ObjectTools::AddExtraObjectsToDelete(ToDelete);
				int32 NumAssetsDeleted = 0;
				
				if (bUseForceDelete)
				{
					NumAssetsDeleted = UExEditorEngineLibrary::DeleteObjects(ToDelete, bUseForceDelete);
				}
				else
				{
					NumAssetsDeleted = ObjectTools::DeleteObjects(ToDelete, bShowConfirmation);
				}
				
				if (NumAssetsDeleted == ToDelete.Num())
				{
					// Successfully deleted all assets in the specified path. Allow the folder to be removed.
					bAllowFolderDelete = true;
				}
				else
				{
					// Not all the assets in the selected paths were deleted
				}
			}
			else
			{
				// Not all the assets in the selected paths were loaded
			}
		}
		else
		{
			// The user declined to load some assets or some assets failed to load
		}
	}

	if (bAllowFolderDelete)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

		for (const FString& PathToDelete : PathsToDelete)
		{
			if (UExEditorEngineLibrary::DeleteEmptyFolderFromDisk(PathToDelete))
			{
				AssetRegistryModule.Get().RemovePath(PathToDelete);
			}
		}

		return true;
	}
	return false;
}

bool UExEditorEngineLibrary::DeleteEmptyFolderFromDisk(const FString& InPathToDelete)
{
	struct FEmptyFolderVisitor : public IPlatformFile::FDirectoryVisitor
	{
		bool bIsEmpty;

		FEmptyFolderVisitor()
			: bIsEmpty(true)
		{
		}

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			if (!bIsDirectory)
			{
				bIsEmpty = false;
				return false; // abort searching
			}

			return true; // continue searching
		}
	};

	FString PathToDeleteOnDisk;
	if (FPackageName::TryConvertLongPackageNameToFilename(InPathToDelete, PathToDeleteOnDisk))
	{
		// Look for files on disk in case the folder contains things not tracked by the asset registry
		FEmptyFolderVisitor EmptyFolderVisitor;
		IFileManager::Get().IterateDirectoryRecursively(*PathToDeleteOnDisk, EmptyFolderVisitor);

		if (EmptyFolderVisitor.bIsEmpty)
		{
			return IFileManager::Get().DeleteDirectory(*PathToDeleteOnDisk, false, true);
		}
	}

	return false;
}

bool UExEditorEngineLibrary::GenerateLODLevels(UWorld* MainWorld, TArray<ULevel*> InSubLevelList, int32 TargetLODIndex)
{
	if (!MainWorld || !MainWorld->WorldComposition)
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s error, World Must Be World Composition"), *FString(__FUNCTION__));
		return false;
	}

	TMap<FName, FWorldCompositionTile*> WorldCompsitionTileMap = UExEditorToolsLibrary::GetWorldCompsitionTile(MainWorld);

	IMeshReductionModule& ReductionModule = FModuleManager::Get().LoadModuleChecked<IMeshReductionModule>("MeshReductionInterface");

	// Select tiles that can be processed
	TArray<ULevel*> TilesToProcess;

	for (ULevel* Level : InSubLevelList)
	{
		if (!Level) continue;

		FName LevelPackageName = Level->GetPackage()->GetFName();
		if (!WorldCompsitionTileMap.Contains(LevelPackageName))
		{
			continue;
		}

		FWorldCompositionTile* Tile = WorldCompsitionTileMap[LevelPackageName];
		if (!Tile->Info.LODList.IsValidIndex(TargetLODIndex))
		{
			continue;
		}

		TilesToProcess.Add(Level);
	}

	// TODO: Need to check out all LOD maps here

	FScopedSlowTask GenerateLODProgress(TilesToProcess.Num(), LOCTEXT("GenerateLODLevel", "Generate LOD Level"));
	GenerateLODProgress.MakeDialog();

	// Generate LOD maps for each tile
	for (ULevel* Level : TilesToProcess)
	{
		FName LevelPackageName = Level->GetPackage()->GetFName();
		FWorldCompositionTile* Tile = WorldCompsitionTileMap[LevelPackageName];

		GenerateLODProgress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Generate Level[%s] LODIndex[%d] Actors[%d]"), *FPackageName::GetShortName(LevelPackageName), TargetLODIndex, Level->Actors.Num())));

		TArray<AActor*>				Actors;
		TArray<ALandscapeProxy*>	LandscapeActors;
		// Separate landscape actors from all others
		for (AActor* Actor : Level->Actors)
		{
			if (Actor)
			{
				ALandscapeProxy* LandscapeProxy = Cast<ALandscapeProxy>(Actor);
				if (LandscapeProxy)
				{
					LandscapeActors.Add(LandscapeProxy);
				}
				else if (!Actor->IsA<AInstancedFoliageActor>()) // Exclude Foliage from merged mesh
				{
					Actors.Add(Actor);
				}
			}
		}

		// Check if we can simplify this level
		IMeshMerging* MeshMerging = ReductionModule.GetMeshMergingInterface();
		if (MeshMerging == nullptr && LandscapeActors.Num() == 0)
		{
			continue;
		}

		// We have to make original level visible, to correctly export it
		const bool bVisibleLevel = FLevelUtils::IsLevelVisible(Level);
		if (!bVisibleLevel)
		{
			MainWorld->WorldComposition->bTemporarilyDisableOriginTracking = true;
			EditorLevelUtils::SetLevelVisibility(Level, true, true);
		}

		FLevelSimplificationDetails SimplificationDetails = Level->LevelSimplification[TargetLODIndex];

		// Source level package name
		FString SourceLongPackageName = Level->GetPackage()->GetName();
		FString SourceShortPackageName = FPackageName::GetShortName(SourceLongPackageName);
		// Target PackageName for generated level: /LongPackageName+LOD/ShortPackageName_LOD[LODIndex]
		const FString LODLevelPackageName = FString::Printf(TEXT("%sLOD/%s_LOD%d"), *SourceLongPackageName, *SourceShortPackageName, TargetLODIndex + 1);

		// Create a package for a LOD level
		UPackage* LODPackage = CreatePackage(*LODLevelPackageName);
		LODPackage->FullyLoad();
		LODPackage->Modify();
		// This is a hack to avoid save file dialog when we will be saving LOD map package
		FPackagePath LODLevelPackagePath = FPackagePath::FromPackageNameChecked(LODLevelPackageName);
		LODLevelPackagePath.SetHeaderExtension(EPackageExtension::Map);
		LODPackage->SetLoadedPath(LODLevelPackagePath);

		// This is current actors offset from their original position
		FVector ActorsOffset = FVector(Tile->Info.AbsolutePosition - MainWorld->OriginLocation);
		if (MainWorld->WorldComposition->bTemporarilyDisableOriginTracking)
		{
			ActorsOffset = FVector::ZeroVector;
		}

		struct FAssetToSpawnInfo
		{
			FAssetToSpawnInfo(UStaticMesh* InStaticMesh, const FTransform& InTransform, ALandscapeProxy* InSourceLandscape = nullptr, int32 InLandscapeLOD = 0)
				: StaticMesh(InStaticMesh)
				, Transform(InTransform)
				, SourceLandscape(InSourceLandscape)
				, LandscapeLOD(InLandscapeLOD)
			{}

			UStaticMesh* StaticMesh;
			FTransform Transform;
			ALandscapeProxy* SourceLandscape;
			int32 LandscapeLOD;
		};

		TArray<FAssetToSpawnInfo>	AssetsToSpawn;
		TArray<UObject*>			GeneratedAssets;

		// Where generated assets will be stored
		UPackage* AssetsOuter = SimplificationDetails.bCreatePackagePerAsset ? nullptr : LODPackage;
		// In case we don't have outer generated assets should have same path as LOD level
		const FString AssetsPath = SimplificationDetails.bCreatePackagePerAsset ? FPackageName::GetLongPackagePath(LODLevelPackageName) + TEXT("/") : TEXT("");

		// Generate Proxy LOD mesh for all actors excluding landscapes
		if (Actors.Num() && MeshMerging != nullptr)
		{
			FMeshProxySettings ProxySettings;
			ProxySettings.ScreenSize = ProxySettings.ScreenSize * (SimplificationDetails.DetailsPercentage / 100.f);
			ProxySettings.MaterialSettings = SimplificationDetails.StaticMeshMaterialSettings;

			FString ProxyPackageName = FString::Printf(TEXT("PROXY_%s_LOD%d"), *FPackageName::GetShortName(Tile->PackageName), TargetLODIndex + 1);

			// Generate proxy mesh and proxy material assets 
			FCreateProxyDelegate ProxyDelegate;
			ProxyDelegate.BindLambda(
				[&](const FGuid Guid, TArray<UObject*>& AssetsToSync)
				{
					//Update the asset registry that a new static mash and material has been created
					if (AssetsToSync.Num())
					{
						UStaticMesh* ProxyMesh = nullptr;
						if (AssetsToSync.FindItemByClass(&ProxyMesh))
						{
							new(AssetsToSpawn)FAssetToSpawnInfo(ProxyMesh, FTransform(-ActorsOffset));
						}

						GeneratedAssets.Append(AssetsToSync);
					}
				});

			FGuid JobGuid = FGuid::NewGuid();

			const IMeshMergeUtilities& MergeUtilities = FModuleManager::Get().LoadModuleChecked<IMeshMergeModule>("MeshMergeUtilities").GetUtilities();
			MergeUtilities.CreateProxyMesh(Actors, ProxySettings, AssetsOuter, AssetsPath + ProxyPackageName, JobGuid, ProxyDelegate);
		}

		// Convert landscape actors into static meshes
		int32 LandscapeActorIndex = 0;
		for (ALandscapeProxy* Landscape : LandscapeActors)
		{
			FFlattenMaterial LandscapeFlattenMaterial;
			FVector LandscapeWorldLocation = Landscape->GetActorLocation();

			int32 LandscapeLOD = SimplificationDetails.LandscapeExportLOD;
			if (!SimplificationDetails.bOverrideLandscapeExportLOD)
			{
				LandscapeLOD = Landscape->MaxLODLevel >= 0 ? Landscape->MaxLODLevel : FMath::CeilLogTwo(Landscape->SubsectionSizeQuads + 1) - 1;
			}

			// Filter out primitives for landscape texture flattening
			TSet<FPrimitiveComponentId> PrimitivesToHide;
			for (TObjectIterator<UPrimitiveComponent> It; It; ++It)
			{
				UPrimitiveComponent* PrimitiveComp = *It;
				UObject* PrimitiveOuter = PrimitiveComp->GetOuter();

				const bool bTargetPrim =
					(PrimitiveComp->GetOuter() == Landscape && !(!SimplificationDetails.bBakeGrassToLandscape && PrimitiveComp->IsA(UHierarchicalInstancedStaticMeshComponent::StaticClass()))) ||
					(SimplificationDetails.bBakeFoliageToLandscape && PrimitiveOuter->IsA(AInstancedFoliageActor::StaticClass()));

				if (!bTargetPrim && PrimitiveComp->IsRegistered() && PrimitiveComp->SceneProxy)
				{
					PrimitivesToHide.Add(PrimitiveComp->SceneProxy->GetPrimitiveComponentId());
				}
			}

			if (SimplificationDetails.bBakeGrassToLandscape)
			{
				/* Flush existing grass components, but not grass maps */
				Landscape->FlushGrassComponents(nullptr, false);
				TArray<FVector> Cameras;
				int32 NumCompsCreated = 0;
				Landscape->UpdateGrass(Cameras, NumCompsCreated, true);
			}

#if WITH_PHOTON_LANDSCAPE
			// Use terrain actor instead of landscape proxy to export flatten material
			FTerrainEditorUtils TerrainEditorUtils;
			UMaterialInterface* LandscapeMaterial = Landscape->GetLandscapeMaterial(LandscapeLOD);
			ATerrainActor* TerrainActor = TerrainEditorUtils.GenerateTerrainFromLandscapeProxy(MainWorld, Landscape, LandscapeMaterial, LandscapeLOD + 1);
#endif

			// This is texture resolution for a landscape mesh, probably needs to be calculated using landscape size
			LandscapeFlattenMaterial.SetPropertySize(EFlattenMaterialProperties::Diffuse, SimplificationDetails.LandscapeMaterialSettings.TextureSize);
			LandscapeFlattenMaterial.SetPropertySize(EFlattenMaterialProperties::Normal, SimplificationDetails.LandscapeMaterialSettings.bNormalMap ? SimplificationDetails.LandscapeMaterialSettings.TextureSize : FIntPoint::ZeroValue);
			LandscapeFlattenMaterial.SetPropertySize(EFlattenMaterialProperties::Metallic, SimplificationDetails.LandscapeMaterialSettings.bMetallicMap ? SimplificationDetails.LandscapeMaterialSettings.TextureSize : FIntPoint::ZeroValue);
			LandscapeFlattenMaterial.SetPropertySize(EFlattenMaterialProperties::Roughness, SimplificationDetails.LandscapeMaterialSettings.bRoughnessMap ? SimplificationDetails.LandscapeMaterialSettings.TextureSize : FIntPoint::ZeroValue);
			LandscapeFlattenMaterial.SetPropertySize(EFlattenMaterialProperties::Specular, SimplificationDetails.LandscapeMaterialSettings.bSpecularMap ? SimplificationDetails.LandscapeMaterialSettings.TextureSize : FIntPoint::ZeroValue);

			FMaterialUtilities::ExportLandscapeMaterial(Landscape, PrimitivesToHide, LandscapeFlattenMaterial);

			if (SimplificationDetails.bBakeGrassToLandscape)
			{
				Landscape->FlushGrassComponents(); // wipe this and let it fix itself later
			}
			FString LandscapeBaseAssetName = FString::Printf(TEXT("%s_LOD%d"), *Landscape->GetName(), TargetLODIndex + 1);
			// Construct landscape material
			UMaterial* StaticLandscapeMaterial = FMaterialUtilities::CreateMaterial(
				LandscapeFlattenMaterial, AssetsOuter, *(AssetsPath + LandscapeBaseAssetName), RF_Public | RF_Standalone, SimplificationDetails.LandscapeMaterialSettings, GeneratedAssets);
			// Currently landscape exports world space normal map
			StaticLandscapeMaterial->bTangentSpaceNormal = false;
			StaticLandscapeMaterial->PostEditChange();

			// Construct landscape static mesh
			FString LandscapeMeshAssetName = TEXT("SM_") + LandscapeBaseAssetName;
			UPackage* MeshOuter = AssetsOuter;
			if (SimplificationDetails.bCreatePackagePerAsset)
			{
				MeshOuter = CreatePackage(*(AssetsPath + LandscapeMeshAssetName));
				MeshOuter->FullyLoad();
				MeshOuter->Modify();
			}

			auto StaticMesh = NewObject<UStaticMesh>(MeshOuter, *LandscapeMeshAssetName, RF_Public | RF_Standalone);
			{
				StaticMesh->InitResources();
				// make sure it has a new lighting guid
				StaticMesh->SetLightingGuid();

				// Set it to use textured lightmaps. Note that Build Lighting will do the error-checking (texcoordindex exists for all LODs, etc).
				StaticMesh->SetLightMapResolution(64);
				StaticMesh->SetLightMapCoordinateIndex(1);

				FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
				/*Don't allow the engine to recalculate normals*/
				SrcModel.BuildSettings.bRecomputeNormals = false;
				SrcModel.BuildSettings.bRecomputeTangents = false;
				SrcModel.BuildSettings.bRemoveDegenerates = false;
				SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
				SrcModel.BuildSettings.bUseFullPrecisionUVs = false;

				//Assign the proxy material to the static mesh
				StaticMesh->GetStaticMaterials().Add(FStaticMaterial(StaticLandscapeMaterial));

				//Set the Imported version before calling the build
				StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
			}
			FMeshDescription* LandscapeRawMesh = StaticMesh->CreateMeshDescription(0);
			FStaticMeshAttributes Attributes(*LandscapeRawMesh);

#if WITH_PHOTON_LANDSCAPE
			// Export mesh description from terrain actor
			TerrainEditorUtils.ExportToRawMesh(TerrainActor, LandscapeLOD, *LandscapeRawMesh);
#else
			Landscape->ExportToRawMesh(LandscapeLOD, *LandscapeRawMesh);
#endif

			TVertexAttributesRef<FVector3f> VertexPositions = Attributes.GetVertexPositions();
			for (const FVertexID VertexID : LandscapeRawMesh->Vertices().GetElementIDs())
			{
				VertexPositions[VertexID] -= (FVector3f)LandscapeWorldLocation;
			}

			//Commit raw mesh and build the staticmesh
			{
				StaticMesh->CommitMeshDescription(0);
				StaticMesh->Build();
				StaticMesh->PostEditChange();
			}

			GeneratedAssets.Add(StaticMesh);
			new(AssetsToSpawn) FAssetToSpawnInfo(StaticMesh, FTransform(LandscapeWorldLocation - ActorsOffset), Landscape, LandscapeLOD);

			LandscapeActorIndex++;
		}

		// Restore level original visibility
		if (!bVisibleLevel)
		{
			EditorLevelUtils::SetLevelVisibility(Level, false, true);
			MainWorld->WorldComposition->bTemporarilyDisableOriginTracking = false;
		}

		if (AssetsToSpawn.Num())
		{
			// Save generated assets
			if (SimplificationDetails.bCreatePackagePerAsset && GeneratedAssets.Num())
			{
				const bool bCheckDirty = false;
				const bool bPromptToSave = false;
				TArray<UPackage*> PackagesToSave;

				for (UObject* Asset : GeneratedAssets)
				{
					FAssetRegistryModule::AssetCreated(Asset);
					GEditor->BroadcastObjectReimported(Asset);
					PackagesToSave.Add(Asset->GetOutermost());
				}

				FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, bCheckDirty, bPromptToSave);

				// Also notify the content browser that the new assets exists
				//FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
				//ContentBrowserModule.Get().SyncBrowserToAssets(GeneratedAssets, true);
			}

			// Create new level and spawn generated assets in it
			UWorld* LODWorld = UWorld::FindWorldInPackage(LODPackage);
			if (LODWorld)
			{
				LODWorld->ClearFlags(RF_Public | RF_Standalone);
				LODWorld->DestroyWorld(false);
				LODWorld->Rename(nullptr, GetTransientPackage());
			}

			// Create a new world
			LODWorld = UWorld::CreateWorld(EWorldType::None, false, FPackageName::GetShortFName(LODPackage->GetFName()), LODPackage);
			LODWorld->SetFlags(RF_Public | RF_Standalone);

			for (FAssetToSpawnInfo& AssetInfo : AssetsToSpawn)
			{
				FVector Location = AssetInfo.Transform.GetLocation();
				FRotator Rotation(ForceInit);

				if (AssetInfo.SourceLandscape != nullptr)
				{
					ALandscapeMeshProxyActor* MeshActor = LODWorld->SpawnActor<ALandscapeMeshProxyActor>(Location, Rotation);
					MeshActor->GetLandscapeMeshProxyComponent()->InitializeForLandscape(AssetInfo.SourceLandscape, AssetInfo.LandscapeLOD);
					MeshActor->GetLandscapeMeshProxyComponent()->SetStaticMesh(AssetInfo.StaticMesh);
					MeshActor->SetActorLabel(AssetInfo.SourceLandscape->GetName());
				}
				else
				{
					AStaticMeshActor* MeshActor = LODWorld->SpawnActor<AStaticMeshActor>(Location, Rotation);
					MeshActor->GetStaticMeshComponent()->SetStaticMesh(AssetInfo.StaticMesh);
					MeshActor->SetActorLabel(AssetInfo.StaticMesh->GetName());
				}
			}

			// Save generated level
			if (FEditorFileUtils::PromptToCheckoutLevels(false, LODWorld->PersistentLevel))
			{
				FEditorFileUtils::SaveLevel(LODWorld->PersistentLevel, LODLevelPackagePath.GetLocalFullPath());
				FAssetRegistryModule::AssetCreated(LODWorld);
			}

			// Destroy the new world we created and collect the garbage
			LODWorld->ClearFlags(RF_Public | RF_Standalone);
			LODWorld->DestroyWorld(false);
			// Also, make sure to release generated assets
			for (UObject* Asset : GeneratedAssets)
			{
				Asset->ClearFlags(RF_Standalone);
			}
			CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
		}
	}

	// Rescan world root
	MainWorld->WorldComposition->Rescan();

	return true;

}

int32 UExEditorEngineLibrary::DeleteObjects(const TArray< UObject* >& InObjectsToDelete, bool bUseForceDelete, bool bAllowCancel)
{
	const FScopedBusyCursor BusyCursor;

	TArray<UObject*> ObjectsToDelete = InObjectsToDelete;
	ObjectTools::AddExtraObjectsToDelete(ObjectsToDelete);

	// Allows deleting of sounds after they have been previewed
	GEditor->ClearPreviewComponents();

	// Ensure the audio manager is not holding on to any sounds
	FAudioDeviceManager* AudioDeviceManager = GEditor->GetAudioDeviceManager();
	if (AudioDeviceManager != nullptr)
	{
		AudioDeviceManager->UpdateActiveAudioDevices(false);

		const int32 NumAudioDevices = AudioDeviceManager->GetNumActiveAudioDevices();
		for (int32 DeviceIndex = 0; DeviceIndex < NumAudioDevices; DeviceIndex++)
		{
			FAudioDevice* AudioDevice = AudioDeviceManager->GetAudioDeviceRaw(DeviceIndex);
			if (AudioDevice != nullptr)
			{
				AudioDevice->StopAllSounds();
			}
		}
	}

	// Query delegate hook to validate if the delete operation is available
	FCanDeleteAssetResult CanDeleteResult;
	FEditorDelegates::OnAssetsCanDelete.Broadcast(ObjectsToDelete, CanDeleteResult);
	if (!CanDeleteResult.Get())
	{
		FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("UnrealEd", "CannotDelete", "Cannot currently delete selected objects. See log for details."));
		return 0;
	}

	// Make sure packages being saved are fully loaded.
	if (!UExEditorEngineLibrary::HandleFullyLoadingPackages(ObjectsToDelete, NSLOCTEXT("UnrealEd", "Delete", "Delete")))
	{
		return 0;
	}

	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// Don't delete anything if we're still building the asset registry, warn the user and don't delete.
	if (AssetRegistryModule.Get().IsLoadingAssets())
	{
		FNotificationInfo Info(NSLOCTEXT("UnrealEd", "Warning_CantDeleteRebuildingAssetRegistry", "Unable To Delete While Discovering Assets"));
		Info.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
		return 0;
	}

	if (UExEditorEngineLibrary::ContainsWorldInUse(ObjectsToDelete))
	{
		FText Title = NSLOCTEXT("UnrealEd", "DeleteFailedWorldInUseTitle", "Unable to delete level");
		FMessageDialog::Open(
			EAppMsgType::Ok,
			NSLOCTEXT("UnrealEd", "DeleteFailedWorldInUse", "Unable to delete level while it is open"),
			&Title
		);

		return 0;
	}

	// let systems clean up any unnecessary references that they may have
	// (so that they're not flagged in the dialog)
	FEditorDelegates::OnAssetsPreDelete.Broadcast(ObjectsToDelete);

	TSharedRef<FAssetDeleteModel> DeleteModel = MakeShared<FAssetDeleteModel>(ObjectsToDelete);

	bool bUserCanceled = false;
	GWarn->BeginSlowTask(NSLOCTEXT("UnrealEd", "VerifyingDelete", "Verifying Delete"), true, bAllowCancel);
	while (!bUserCanceled && DeleteModel->GetState() != FAssetDeleteModel::Finished)
	{
		DeleteModel->Tick(0);
		GWarn->StatusUpdate((int32)(DeleteModel->GetProgress() * 100), 100, DeleteModel->GetProgressText());

		if (bAllowCancel)
		{
			bUserCanceled = GWarn->ReceivedUserCancel();
		}
	}
	GWarn->EndSlowTask();

	if (bUserCanceled)
	{
		EXEDITORTOOLS_LOG(Warning, TEXT("User canceled delete operation"));
		return 0;
	}

	//强制删除
	if (bUseForceDelete && DeleteModel->CanForceDelete())
	{ 
		if (!DeleteModel->DoForceDelete())
		{
			EXEDITORTOOLS_LOG(Error, TEXT("%s Force Delete Error"), *FString(__FUNCTION__));
		}
	}
	else if (!DeleteModel->DoDelete())
	{
		EXEDITORTOOLS_LOG(Error, TEXT("%s Delete Error"), *FString(__FUNCTION__));
	}

	return DeleteModel->GetDeletedObjectCount();
}

bool UExEditorEngineLibrary::ContainsWorldInUse(const TArray< UObject* >& ObjectsToDelete)
{
	TArray<const UWorld*> WorldsToDelete;

	for (const UObject* ObjectToDelete : ObjectsToDelete)
	{
		if (const UWorld* World = Cast<UWorld>(ObjectToDelete))
		{
			WorldsToDelete.AddUnique(World);
		}
	}

	if (WorldsToDelete.Num() == 0)
	{
		return false;
	}

	auto GetCombinedWorldNames = [](const TArray<const UWorld*>& Worlds) -> FString
	{
		return FString::JoinBy(Worlds, TEXT(", "),
			[](const UWorld* World) -> FString
			{
				return World->GetPathName();
			});
	};

	EXEDITORTOOLS_LOG(Log, TEXT("Deleting %d worlds: %s"), WorldsToDelete.Num(), *GetCombinedWorldNames(WorldsToDelete));

	TArray<const UWorld*> ActiveWorlds;

	for (const FWorldContext& WorldContext : GEditor->GetWorldContexts())
	{
		if (const UWorld* World = WorldContext.World())
		{
			ActiveWorlds.AddUnique(World);

			for (const ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
			{
				if (StreamingLevel && StreamingLevel->GetLoadedLevel() && StreamingLevel->GetLoadedLevel()->GetOuter())
				{
					if (const UWorld* StreamingWorld = Cast<UWorld>(StreamingLevel->GetLoadedLevel()->GetOuter()))
					{
						ActiveWorlds.AddUnique(StreamingWorld);
					}
				}
			}
		}
	}

	EXEDITORTOOLS_LOG(Log, TEXT("Currently %d active worlds: %s"), ActiveWorlds.Num(), *GetCombinedWorldNames(ActiveWorlds));

	for (const UWorld* World : WorldsToDelete)
	{
		if (ActiveWorlds.Contains(World))
		{
			return true;
		}
	}

	return false;
}


bool UExEditorEngineLibrary::HandleFullyLoadingPackages(const TArray<UObject*>& Objects, const FText& OperationText)
{
	// Get list of outermost packages.
	TArray<UPackage*> TopLevelPackages;
	for (int32 ObjectIndex = 0; ObjectIndex < Objects.Num(); ObjectIndex++)
	{
		UObject* Object = Objects[ObjectIndex];
		if (Object)
		{
			TopLevelPackages.AddUnique(Object->GetOutermost());
		}
	}

	return UPackageTools::HandleFullyLoadingPackages(TopLevelPackages, OperationText);
}
#undef LOCTEXT_NAMESPACE