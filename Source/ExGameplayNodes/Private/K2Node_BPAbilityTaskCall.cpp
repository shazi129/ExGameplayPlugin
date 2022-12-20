
#include "K2Node_BPAbilityTaskCall.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/CompilerResultsLog.h"
#include "K2Node_Self.h"

UK2Node_BPAbilityTaskCall::UK2Node_BPAbilityTaskCall(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == true)
	{
		UK2Node_BPAbilityTaskCall::RegisterSpecializedTaskNodeClass(GetClass());
	}
}


bool UK2Node_BPAbilityTaskCall::IsCompatibleWithGraph(UEdGraph const* TargetGraph) const
{
	bool bIsCompatible = Super::IsCompatibleWithGraph(TargetGraph);
	if (bIsCompatible)
	{
		return true;
	}

	EGraphType GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
	bool const bAllowLatentFuncs = (GraphType == GT_Ubergraph || GraphType == GT_Macro);

	if (bAllowLatentFuncs)
	{
		UBlueprint* MyBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
		if (MyBlueprint && MyBlueprint->GeneratedClass)
		{
			if (MyBlueprint->GeneratedClass->IsChildOf(UAbilityTask::StaticClass()))
			{
				bIsCompatible = true;
			}
		}
	}

	return bIsCompatible;

}

void UK2Node_BPAbilityTaskCall::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static void SetNodeFunc(UEdGraphNode* NewNode, bool /*bIsTemplateNode*/, TWeakObjectPtr<UFunction> FunctionPtr)
		{
			UK2Node_BPAbilityTaskCall* AsyncTaskNode = CastChecked<UK2Node_BPAbilityTaskCall>(NewNode);
			if (FunctionPtr.IsValid())
			{
				UFunction* Func = FunctionPtr.Get();
				FObjectProperty* ReturnProp = CastFieldChecked<FObjectProperty>(Func->GetReturnProperty());

				AsyncTaskNode->ProxyFactoryFunctionName = Func->GetFName();
				AsyncTaskNode->ProxyFactoryClass = Func->GetOuterUClass();
				AsyncTaskNode->ProxyClass = ReturnProp->PropertyClass;
			}
		}
	};

	UClass* NodeClass = GetClass();
	ActionRegistrar.RegisterClassFactoryActions<UAbilityTask>(FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateLambda([NodeClass](const UFunction* FactoryFunc)->UBlueprintNodeSpawner*
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintFunctionNodeSpawner::Create(FactoryFunc);
			check(NodeSpawner != nullptr);
			NodeSpawner->NodeClass = NodeClass;

			TWeakObjectPtr<UFunction> FunctionPtr = MakeWeakObjectPtr(const_cast<UFunction*>(FactoryFunc));
			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(GetMenuActions_Utils::SetNodeFunc, FunctionPtr);

			return NodeSpawner;
		}));
}

void UK2Node_BPAbilityTaskCall::PostCreateProxyObjectNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UK2Node_CallFunction* FactoryFunctionNode)
{
	UBlueprint* MyBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(SourceGraph);
	if (!MyBlueprint || !MyBlueprint->GeneratedClass || !MyBlueprint->GeneratedClass->IsChildOf(UAbilityTask::StaticClass()))
	{
		return;
	}

	UFunction* Function = FactoryFunctionNode->GetTargetFunction();
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

	//遍历函数属性, 找到DefalutToSelf的Pin
	TSet<FName> PinsDefalutToSelf;
	for (TFieldIterator<FProperty> PropIt(Function); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	{
		FProperty* Param = *PropIt;
		const bool bIsFunctionInput = !Param->HasAnyPropertyFlags(CPF_ReturnParm) && (!Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm));

		if (bIsFunctionInput)
		{
			const FString PinNameStr = Param->GetFName().ToString();
			const FString& DefaultToSelfMetaValue = Function->GetMetaData(FBlueprintMetadata::MD_DefaultToSelf);
			if (PinNameStr == DefaultToSelfMetaValue)
			{
				UEdGraphPin* DefaultToSelfPin = FactoryFunctionNode->FindPin(PinNameStr);

				// 从task中获取ability的节点
				UK2Node_CallFunction* GetAbilityNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
				//const FName GetAbilityNodeFuncName = GET_FUNCTION_NAME_CHECKED(UMOAbilityBPLibrary, GetAbilityFromTask);
				//GetAbilityNode->FunctionReference.SetExternalMember(GetAbilityNodeFuncName, UMOAbilityBPLibrary::StaticClass());
				GetAbilityNode->AllocateDefaultPins();
				UEdGraphPin* InputTaskPin = GetAbilityNode->FindPinChecked(TEXT("Task"));

				//self节点
				UK2Node_Self * SelfRefNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
				SelfRefNode->AllocateDefaultPins();

				//连接节点
				bool bIsErrorFree = Schema->TryCreateConnection(SelfRefNode->FindPinChecked(Schema->PN_Self), InputTaskPin);
				bIsErrorFree &= Schema->TryCreateConnection(GetAbilityNode->GetReturnValuePin(), DefaultToSelfPin);

				if (!bIsErrorFree)
				{
					CompilerContext.MessageLog.Error(TEXT("UK2Node_BPAbilityTaskCall::PostCreateProxyObjectNode error"), this);
				}
			}
		}
	}
}