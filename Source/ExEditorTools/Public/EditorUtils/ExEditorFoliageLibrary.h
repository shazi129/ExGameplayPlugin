#pragma once

#include "CoreMinimal.h"
#include "InstancedFoliageActor.h"
#include "ExEditorFoliageLibrary.generated.h"

/*
* 编辑器植被操作相关工具函数
*/

UCLASS()
class EXEDITORTOOLS_API UExEditorFoliageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static AInstancedFoliageActor* GetInstancedFoliageActorForLevel(ULevel* InLevel, bool bCreateIfNone = false);

	//获取一个Actor被植被引用到的Component
	static TArray<UPrimitiveComponent*> GetFoliageReferencedComponent(AInstancedFoliageActor* IFA, AActor* Actor);

	static void RemoveFoliageInstanceForComponent(AInstancedFoliageActor* IFA, UPrimitiveComponent* InNewComponent);

};