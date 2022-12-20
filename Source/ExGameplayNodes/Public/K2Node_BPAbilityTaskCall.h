#pragma once

#include "K2Node.h"
#include "KismetCompiler.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"	

//#include "K2Node_LatentAbilityCall.h"
#include "K2Node_LatentGameplayTaskCall.h"
#include "K2Node_BPAbilityTaskCall.generated.h"

class FBlueprintActionDatabaseRegistrar;

UCLASS()
class UK2Node_BPAbilityTaskCall : public UK2Node_LatentGameplayTaskCall//UK2Node_LatentAbilityCall
{
	GENERATED_BODY()

public:
	UK2Node_BPAbilityTaskCall(const FObjectInitializer& ObjectInitializer);

	// UEdGraphNode interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsCompatibleWithGraph(UEdGraph const* TargetGraph) const override;
	// End of UEdGraphNode interface

	virtual void PostCreateProxyObjectNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, class UK2Node_CallFunction* FactoryFunctionNode);// override;
};
