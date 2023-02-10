#pragma once 

#include "CoreMinimal.h"
#include "Engine/WorldComposition.h"
#include "ExTypes.h"
#include "ExEditorLevelLibrary.generated.h"

/*
* 编辑器关卡相关的操作函数
*/

USTRUCT(BlueprintType)
struct FSubLevelPriority
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int LoadPriority;
};

UCLASS()
class EXEDITORTOOLS_API UExEditorLevelLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "External Level Tools")
		static bool CopyWorldActors(const FString& SrcWorldPath, const FString& DescWorldPath);

	UFUNCTION(BlueprintCallable, Category = "External Level Tools")
		static UWorld* CreateWorld(FString WorldPath, FString WorldName);

	UFUNCTION(BlueprintPure, Category = "External Level Tools")
		static UWorld* LoadWorldFromPath(const FString& PackagePath);

	static TMap<FName, FWorldCompositionTile*> GetWorldCompsitionTile(UWorld* World);

	UFUNCTION(BlueprintPure, Category = "External Level Tools")
		static TArray<ULevel*> FindSubLevel(UWorld* World, TArray<FName> LevelPackageName);

	UFUNCTION(BlueprintCallable, Category = "External Level Tools")
		static bool UpdateLevelTileInfo(UWorld* World, TArray<FName> LevelPackageName, FBPTiledWorldInfo TileInfo);

	UFUNCTION(BlueprintCallable, Category = "External Level Tools")
		static bool ReorderSubLevels(UWorld* World, TMap<FName, int> SubLevelPriorities);
};