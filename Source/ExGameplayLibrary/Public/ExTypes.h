/*
 * 这个文件主要是对引擎中一些变量的扩展，使其可以在蓝图中使用或配置
 */

#pragma once

#include "ExTypes.generated.h"

/*
* ENetMode
* EngineBaseTypes.h
*/
UENUM(Blueprintable)
enum class EBPNetMode : uint8
{
	Standalone = 0					UMETA(DisplayName = "Standalone"),
	DedicatedServer = 1			UMETA(DisplayName = "DedicatedServer"),
	ListenServer = 2				UMETA(DisplayName = "ListenServer"),
	Client = 3						UMETA(DisplayName = "Client"),

	MAX = 4							UMETA(DisplayName = "MAX"),
};

UENUM(Blueprintable)
enum class EBPLogLevel : uint8
{
	Log,
	Warning,
	Error
};

/*
* FWorldTileLayer
* WorldCompositionUtility.h
*/
USTRUCT(BlueprintType)
struct EXGAMEPLAYLIBRARY_API FBPWorldTileLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	int32 StreamingDistance;

	UPROPERTY(EditAnywhere)
	bool DistanceStreamingEnabled;

	bool IsValid() { return !Name.IsEmpty() && StreamingDistance > 0; }
};

/*
* FWorldTileLODInfo
* WorldCompositionUtility.h
*/
USTRUCT(BlueprintType)
struct EXGAMEPLAYLIBRARY_API FBPWorldTileLODInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 RelativeStreamingDistance;

	bool IsValid() { return RelativeStreamingDistance > 0; }
};


/*
* FTiledWorldInfo
* WorldCompositionUtility.h
*/
USTRUCT(BlueprintType)
struct EXGAMEPLAYLIBRARY_API FBPTiledWorldInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FBPWorldTileLayer Layer;

	UPROPERTY(EditAnywhere)
	TArray<FBPWorldTileLODInfo> LODList;
};
