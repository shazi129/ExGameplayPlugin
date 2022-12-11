#include "CoreMinimal.h"
#include "ExEditorLevelLibrary.generated.h"

/*
* 编辑器关卡相关的操作函数
*/

UCLASS()
class EXEDITORTOOLS_API UExEditorLevelLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "External Editor Level Tools")
	static TArray<AActor*> CopyActorsBetweenLevels(ULevel* SrcLevel, ULevel* DestLevel, const TArray<AActor*>& ActorsToMove);
};