#include "EditorUtils/ExEditorFoliageLibrary.h"

AInstancedFoliageActor* UExEditorFoliageLibrary::GetInstancedFoliageActorForLevel(ULevel* InLevel, bool bCreateIfNone)
{
	AInstancedFoliageActor* IFA = nullptr;
	if (InLevel)
	{
		IFA = InLevel->InstancedFoliageActor.Get();

		if (!IFA && bCreateIfNone)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.OverrideLevel = InLevel;
			IFA = InLevel->GetWorld()->SpawnActor<AInstancedFoliageActor>(SpawnParams);
			InLevel->InstancedFoliageActor = IFA;
		}
	}

	return IFA;
}

TArray<UPrimitiveComponent*> UExEditorFoliageLibrary::GetFoliageReferencedComponent(AInstancedFoliageActor* IFA, AActor* Actor)
{
	TArray<UPrimitiveComponent*> Result;

	TInlineComponentArray<UPrimitiveComponent*> Components(Actor);
	for (UPrimitiveComponent* Component : Components)
	{
		const auto baseID = IFA->InstanceBaseCache.GetInstanceBaseId(Component);
		if (baseID != FFoliageInstanceBaseCache::InvalidBaseId)
		{
			Result.Add(Component);
		}
	}

	return Result;
}

void UExEditorFoliageLibrary::RemoveFoliageInstanceForComponent(AInstancedFoliageActor* IFA, UPrimitiveComponent* InNewComponent)
{
	const auto OldBaseId = IFA->InstanceBaseCache.GetInstanceBaseId(InNewComponent);
	if (OldBaseId == FFoliageInstanceBaseCache::InvalidBaseId)
	{
		// This foliage actor has no instances with specified base
		return;
	}

	bool bModified = false;

	//for (auto& Pair : IFA->GetFoliageInfos())
	//{
	//	const TUniqueObj<FFoliageInfo>& Info = Pair.Value;
	//	TSet<int32>* OldInstanceSet = Info.ComponentHash.Find(OldBaseId);
	//	if (OldInstanceSet && OldInstanceSet->Num())
	//	{
	//		TArray<int32> PotentialInstances;
	//		PotentialInstances.SetNum(Info.Instances.Num());
	//		for (int32 InstanceIndex = 0; InstanceIndex < Info.Instances.Num(); ++InstanceIndex)
	//		{
	//			PotentialInstances[InstanceIndex] = InstanceIndex;
	//		}

	//		for (int32 InstanceIndex : PotentialInstances)
	//		{
	//			if (!bModified)
	//			{
	//				IFA->Modify();
	//				bModified = true;
	//			}

	//			if (Info.Instances.IsValidIndex(InstanceIndex) && OldInstanceSet->Contains(InstanceIndex))
	//			{
	//				FFoliageInstance& InstanceToMove = Info.Instances[InstanceIndex];
	//				InstanceToMove.BaseComponent = InNewComponent;
	//				InstanceToMove.BaseId = FFoliageInstanceBaseCache::InvalidBaseId;
	//				OldInstanceSet->Remove(InstanceIndex);
	//			}
	//		}
	//	}
	//}
}
