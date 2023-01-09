#include "ExMeshLibrary.h"
#include "ExGameplayLibraryModule.h"

TArray<FPolygonGroupID> UExMeshLibrary::GetPolygonGroupIDList(UMeshDescriptionBase* MeshDescription)
{
	TArray<FPolygonGroupID> Result;

	if (MeshDescription != nullptr)
	{
		for (const FPolygonGroupID PolygonGroupID : MeshDescription->PolygonGroups().GetElementIDs())
		{
			new (Result)FPolygonGroupID(PolygonGroupID.GetValue());
		}
	}

	return Result;
}

bool UExMeshLibrary::CheckStaticMeshData(UStaticMesh* StaticMesh, FString& ErrMsg)
{
	if (StaticMesh == nullptr)
	{
		ErrMsg = "Mesh is NULL";
		return false;
	}

	FString MeshName = StaticMesh->GetName();

	FStaticMeshRenderData* RenderData = StaticMesh->GetRenderData();
	if (!RenderData)
	{
		ErrMsg = FString::Printf(TEXT("Mesh[%s] Render Data is null"), *MeshName);
		return false;
	}

	int LODNumber = RenderData->LODResources.Num();
	for (int LODIndex = 0; LODIndex < LODNumber; LODIndex++)
	{
		UStaticMeshDescription* Description = StaticMesh->GetStaticMeshDescription(LODIndex);
		FStaticMeshLODResources& LODResource = RenderData->LODResources[LODIndex];

		int SectionNum = LODResource.Sections.Num();
		for (int SectionIndex = 0; SectionIndex < SectionNum; SectionIndex++)
		{
			FStaticMeshSection& MesSection = LODResource.Sections[SectionIndex];
			if (MesSection.NumTriangles == 0)
			{
				ErrMsg = FString::Printf(TEXT("Mesh[%s] LOD[%d] Section[%d] No Triangle"), *MeshName, LODIndex, SectionIndex);
				return false;
			}
		}

		
		
	}

	return true;
}

