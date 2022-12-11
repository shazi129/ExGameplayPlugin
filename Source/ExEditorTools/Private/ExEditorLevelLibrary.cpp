#include "ExEditorLevelLibrary.h"
#include "ExEditorToolsModule.h"
#include "ExEditorEngineLibrary.h"
#include "InstancedFoliageActor.h"

TArray<AActor*> UExEditorLevelLibrary::CopyActorsBetweenLevels(ULevel* SrcLevel, ULevel* DestLevel, const TArray<AActor*>& TargetActors)
{
	TArray<AActor*> PasteActors;
	UExEditorEngineLibrary::CopyOrMoveActorsToLevel(SrcLevel, TargetActors, DestLevel, false, true, true, &PasteActors);

	return PasteActors;
}