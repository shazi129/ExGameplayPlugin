#include "ExEditorToolsLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"

void UExEditorToolsLibrary::MarkBlueprintAsStructurallyModified(UBlueprint* Blueprint)
{
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
}

UWidget* UExEditorToolsLibrary::ClearWidget(UWidgetTree* WidgetTree, FString RootName)
{
	if (RootName.IsEmpty()|| !WidgetTree)
	{
		return nullptr;
	}

	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);

	UWidget* RootWidget = nullptr;

	for (int i = 0; i < Widgets.Num(); i++)
	{
		UWidget* Widget = Widgets[i];

		//看是不是要找的RootWidget
		if (RootWidget == nullptr && Widget->GetName() == RootName)
		{
			RootWidget = Widget;
			continue;
		}

		if (!Widget->Slot || !Widget->Slot->Parent)
		{
			continue;
		}

		//删除指定父节点的Widget
		TObjectPtr<UWidget> Parent = Widget->Slot->Parent;
		if (Parent->GetName() == RootName)
		{
			WidgetTree->RemoveWidget(Widget);
		}
	}

	return RootWidget;
}

USCS_Node* UExEditorToolsLibrary::GetRootNodeByTag(UBlueprint* Blueprint, FName Tag)
{
	if (Blueprint == nullptr || Tag.IsValid() == false)
	{
		return nullptr;
	}

	const TArray<USCS_Node*>& RootNodes = Blueprint->SimpleConstructionScript->GetRootNodes();
	for (int i = 0; i < RootNodes.Num(); i++)
	{
		TObjectPtr<UActorComponent> RootComponent = Cast<UActorComponent>(RootNodes[i]->ComponentTemplate);
		if (RootComponent && RootComponent->ComponentHasTag(Tag))
		{
			return RootNodes[i];
		}
	}

	return nullptr;
}

TArray<UActorComponent*> UExEditorToolsLibrary::GetNodeChildComponentsByTag(USCS_Node* Node, FName Tag)
{
	TArray<UActorComponent*> Result;

	const TArray<USCS_Node*>& ChildNodes = Node->GetChildNodes();
	for (int i = 0; i < ChildNodes.Num(); i++)
	{
		UActorComponent* Component = ChildNodes[i]->ComponentTemplate;
		if (Component != nullptr && Component->ComponentHasTag(Tag))
		{
			Result.Add(Component);
		}
	}

	return Result;
}