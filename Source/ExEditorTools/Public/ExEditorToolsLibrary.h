#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "ExEditorToolsLibrary.generated.h"

UCLASS()
class EXEDITORTOOLS_API UExEditorToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void MarkBlueprintAsStructurallyModified(UBlueprint* Blueprint);

	static UWidget* ClearWidget(UWidgetTree* WidgetTree, FString RootName);

	static USCS_Node* GetRootNodeByTag(UBlueprint* Blueprint, FName Tag);

	static TArray<UActorComponent*> GetNodeChildComponentsByTag(USCS_Node* Node, FName Tag);
};