#pragma once

#include "CoreMinimal.h"
#include "MeshDescription.h"
#include "MeshDescriptionBase.h"
#include "StaticMeshDescription.h"
#include "ExMeshLibrary.generated.h"

UCLASS()
class EXGAMEPLAYLIBRARY_API UExMeshLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Polygon Group")
	static TArray<FPolygonGroupID> GetPolygonGroupIDList(UMeshDescriptionBase* MeshDescription);

	UFUNCTION(BlueprintCallable, Category = "Static Mesh")
	static bool CheckStaticMeshData(UStaticMesh* StaticMesh, FString& ErrMsg);
};