#include "K2Node_RunBPAbilityTask.h"
#include "UObject/UnrealType.h"
#include "Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CallArrayFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "K2Node_Self.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_BaseAsyncTask.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_EnumLiteral.h"
#include "BlueprintCompilationManager.h"
#include "K2Node_AssignmentStatement.h"


static const FName BlueprintPinName(TEXT("Blueprint"));

UK2Node_RunBPAbilityTask::UK2Node_RunBPAbilityTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyActivateFunctionName = GET_FUNCTION_NAME_CHECKED(UGameplayTask, ReadyForActivation);

	//ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UMOAbilityBPLibrary, NewAbilityTask);

	//ProxyFactoryClass = UMOAbilityBPLibrary::StaticClass();
	//TaskClass = UMOBPAbilityTask::StaticClass();

	TaskResultPinName = TEXT("AsyncTask");
	TaskClassPinName = TEXT("BPTaskClass");
	TaskNamePinName = TEXT("InstanceName");
}

FText UK2Node_RunBPAbilityTask::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FText NodeTitle = FText::FromString(TEXT("RunBPAbilityTask"));
	if (TitleType != ENodeTitleType::MenuTitle)
	{
		if (UEdGraphPin* ClassPin = GetClassPin())
		{
			if (ClassPin->LinkedTo.Num() > 0)
			{
				NodeTitle = FText::FromString(TEXT("RunBPAbilityTask"));
			}
			else if (ClassPin->DefaultObject == nullptr)
			{
				NodeTitle = FText::FromString(TEXT("Run None"));
			}
			else
			{
				FText ClassName;
				if (UClass* PickedClass = Cast<UClass>(ClassPin->DefaultObject))
				{
					ClassName = PickedClass->GetDisplayNameText();
				}
				FFormatNamedArguments Args;
				Args.Add(TEXT("ClassName"), ClassName);
				NodeTitle = FText::Format(FText::FromString(TEXT("Run {ClassName}")), Args);
			}
		}
		else
		{
			NodeTitle = FText::FromString(TEXT("Run None"));
		}
	}
	return NodeTitle;
}

FText UK2Node_RunBPAbilityTask::GetTooltipText() const
{
	return FText::FromString(TEXT("create and run a blueprint ability task"));
}

FText UK2Node_RunBPAbilityTask::GetMenuCategory() const
{
	return FText::FromString(TEXT("BPAbilityTask")); 
}

UClass* UK2Node_RunBPAbilityTask::GetClassPinBaseClass() const
{
	return UAbilityTask::StaticClass();
}

void UK2Node_RunBPAbilityTask::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

UFunction* UK2Node_RunBPAbilityTask::GetFactoryFunction() const
{
	FMemberReference FunctionReference;
	FunctionReference.SetExternalMember(ProxyFactoryFunctionName, ProxyFactoryClass);

	UFunction* FactoryFunction = FunctionReference.ResolveMember<UFunction>(GetBlueprint());

	if (FactoryFunction == nullptr)
	{
		FactoryFunction = ProxyFactoryClass->FindFunctionByName(ProxyFactoryFunctionName);
		UE_CLOG(FactoryFunction == nullptr, LogBlueprint, Error, TEXT("FactoryFunction %s null in %s. Was a class deleted or saved on a non promoted build?"), *ProxyFactoryFunctionName.ToString(), *GetFullName());
	}

	return FactoryFunction;
}

bool UK2Node_RunBPAbilityTask::IsCompatibleWithGraph(UEdGraph const* TargetGraph) const
{
	bool bIsCompatible = false;

	EGraphType GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
	bool const bAllowLatentFuncs = (GraphType == GT_Ubergraph || GraphType == GT_Macro);

	if (bAllowLatentFuncs)
	{
		UBlueprint* MyBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
		if (MyBlueprint && MyBlueprint->GeneratedClass)
		{
			if (MyBlueprint->GeneratedClass->IsChildOf(UAbilityTask::StaticClass())
				|| MyBlueprint->GeneratedClass->IsChildOf(UGameplayAbility::StaticClass()))
			{
				bIsCompatible = true;
			}
		}
	}

	return bIsCompatible;

}

void UK2Node_RunBPAbilityTask::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add execution pins
	UEdGraphPin* NodeExecPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* NodeThenPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	//返回的task
	UEdGraphPin* NodeTaskResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, ProxyFactoryClass, TaskResultPinName);

	//task类
	UEdGraphPin* NodeTaskClassPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class, TaskClass, TaskClassPinName);

	//task类名
	UEdGraphPin* NodeTaskNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, TaskNamePinName);

}


bool UK2Node_RunBPAbilityTask::HandleDelegateImplementation
(
	FMulticastDelegateProperty* CurrentProperty,
	UEdGraphPin* CreatedTaskPin,
	UEdGraphPin*& InOutLastThenPin,
	TMap<FName, UK2Node_TemporaryVariable*>& OutInfos,
	UEdGraph* SourceGraph,
	FKismetCompilerContext& CompilerContext
)
{
	UK2Node* CurrentNode = this;

	bool bIsErrorFree = true;
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(CurrentProperty && CreatedTaskPin && InOutLastThenPin && CurrentNode && SourceGraph && Schema);

	UEdGraphPin* PinForCurrentDelegateProperty = CurrentNode->FindPin(CurrentProperty->GetFName());
	if (!PinForCurrentDelegateProperty || (UEdGraphSchema_K2::PC_Exec != PinForCurrentDelegateProperty->PinType.PinCategory))
	{
		FText ErrorMessage = FText::Format(FText::FromString(TEXT("RunBPAbilityTask: Cannot find execution pin for delegate ")), FText::FromString(CurrentProperty->GetName()));
		CompilerContext.MessageLog.Error(*ErrorMessage.ToString(), CurrentNode);
		return false;
	}

	//用一个CustomEvent把delegate连到输出
	UK2Node_CustomEvent* CurrentCENode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(CurrentNode, PinForCurrentDelegateProperty, SourceGraph);
	CurrentCENode->CustomFunctionName = *FString::Printf(TEXT("%s_%s"), *CurrentProperty->GetName(), *CompilerContext.GetGuid(CurrentNode));
	CurrentCENode->AllocateDefaultPins();

	//用AddDelegate把delegate和CustromEvent连起来
	UK2Node_AddDelegate* AddDelegateNode = CompilerContext.SpawnIntermediateNode<UK2Node_AddDelegate>(CurrentNode, SourceGraph);
	AddDelegateNode->SetFromProperty(CurrentProperty, false, CurrentProperty->GetOwnerClass());
	AddDelegateNode->AllocateDefaultPins();

	bIsErrorFree &= Schema->TryCreateConnection(AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), CreatedTaskPin);
	bIsErrorFree &= Schema->TryCreateConnection(InOutLastThenPin, AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
	InOutLastThenPin = AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);

	UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(CurrentNode, SourceGraph);
	SelfNode->AllocateDefaultPins();

	bIsErrorFree &= Schema->TryCreateConnection(AddDelegateNode->GetDelegatePin(), CurrentCENode->FindPin(UK2Node_Event::DelegateOutputName));

// 		UK2Node_CreateDelegate* CreateDelegateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CreateDelegate>(CurrentNode, SourceGraph);
// 		CreateDelegateNode->AllocateDefaultPins();
// 		bIsErrorFree &= Schema->TryCreateConnection(AddDelegateNode->GetDelegatePin(), CreateDelegateNode->GetDelegateOutPin());
// 		bIsErrorFree &= Schema->TryCreateConnection(SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), CreateDelegateNode->GetObjectInPin());
// 		CreateDelegateNode->SetFunction(CurrentCENode->GetFunctionName());

	//输出参数, 直接连到具有相同名字的输出参数上
	for (TFieldIterator<FProperty> PropIt(AddDelegateNode->GetDelegateSignature()); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	{
		const FProperty* Param = *PropIt;
		if (!Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm))
		{
			FEdGraphPinType PinType;
			bIsErrorFree &= Schema->ConvertPropertyToPinType(Param, /*out*/ PinType);

			UEdGraphPin* DelegateParamPin = CurrentCENode->CreateUserDefinedPin(Param->GetFName(), PinType, EGPD_Output);
			bIsErrorFree &= (nullptr != DelegateParamPin);
		}
	}

	//delegate的输出数据
	UEdGraphPin* CustonEventNodeThen = CurrentCENode->FindPinChecked(UEdGraphSchema_K2::PN_Then);

	for (auto OutInfo : OutInfos)
	{
		UEdGraphPin* PinWithData = CurrentCENode->FindPin(OutInfo.Key);
		if (!PinWithData)
		{
			continue;
		}

		UK2Node_AssignmentStatement* AssignNode = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(CurrentNode, SourceGraph);
		AssignNode->AllocateDefaultPins();
		bIsErrorFree &= Schema->TryCreateConnection(CustonEventNodeThen, AssignNode->GetExecPin());
		bIsErrorFree &= Schema->TryCreateConnection(OutInfo.Value->GetVariablePin(), AssignNode->GetVariablePin());
		AssignNode->NotifyPinConnectionListChanged(AssignNode->GetVariablePin());
		bIsErrorFree &= Schema->TryCreateConnection(AssignNode->GetValuePin(), PinWithData);
		AssignNode->NotifyPinConnectionListChanged(AssignNode->GetValuePin());

		CustonEventNodeThen = AssignNode->GetThenPin();
	}

	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*PinForCurrentDelegateProperty, *CustonEventNodeThen).CanSafeConnect();

	return bIsErrorFree;
}

UEdGraphPin* UK2Node_RunBPAbilityTask::GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == TaskClassPinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

bool UK2Node_RunBPAbilityTask::IsSpawnVarPin(UEdGraphPin* Pin) const
{
	return(Pin->PinName != UEdGraphSchema_K2::PN_Execute &&
		Pin->PinName != UEdGraphSchema_K2::PN_Then &&
		Pin->PinName != TaskResultPinName &&
		Pin->PinName != TaskClassPinName &&
		Pin->PinName != TaskNamePinName);
}

void UK2Node_RunBPAbilityTask::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	Super::PinDefaultValueChanged(ChangedPin);
	if (ChangedPin && (ChangedPin->PinName == TaskClassPinName))
	{
		OnClassPinChanged();
	}
}

void UK2Node_RunBPAbilityTask::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && (Pin->PinName == TaskClassPinName))
	{
		OnClassPinChanged();
	}
}

void UK2Node_RunBPAbilityTask::OnClassPinChanged()
{
	// Remove all pins related to archetype variables
	TArray<UEdGraphPin*> OldPins = Pins;
	TArray<UEdGraphPin*> OldClassPins;

	for (UEdGraphPin* OldPin : OldPins)
	{
		if (IsSpawnVarPin(OldPin))
		{
			Pins.Remove(OldPin);
			OldClassPins.Add(OldPin);
		}
	}

	TArray<UEdGraphPin*> NewClassPins;
	if (UClass* UseSpawnClass = GetTaskToCreate())
	{
		CreatePinsForClass(UseSpawnClass, &NewClassPins);
	}

	RestoreSplitPins(OldPins);

	UEdGraphPin* ResultPin = GetResultPin();
	// Cache all the pin connections to the ResultPin, we will attempt to recreate them
	TArray<UEdGraphPin*> ResultPinConnectionList = ResultPin->LinkedTo;
	// Because the archetype has changed, we break the output link as the output pin type will change
	ResultPin->BreakAllPinLinks(true);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Recreate any pin links to the Result pin that are still valid
	for (UEdGraphPin* Connections : ResultPinConnectionList)
	{
		K2Schema->TryCreateConnection(ResultPin, Connections);
	}

	// Rewire the old pins to the new pins so connections are maintained if possible
	RewireOldPinsToNewPins(OldClassPins, Pins, nullptr);

	// Refresh the UI for the graph so the pin changes show up
	GetGraph()->NotifyGraphChanged();

	// Mark dirty
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}


UEdGraphPin* UK2Node_RunBPAbilityTask::GetResultPin() const
{
	UEdGraphPin* Pin = FindPinChecked(TaskResultPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

void UK2Node_RunBPAbilityTask::CreatePinsForClass(UClass* InClass, TArray<UEdGraphPin*>* OutClassPins)
{
	check(InClass);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	const UObject* const ClassDefaultObject = InClass->GetDefaultObject(false);

	//创建expose on spawn的pin
	for (TFieldIterator<FProperty> PropertyIt(InClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;
		UClass* PropertyClass = CastChecked<UClass>(Property->GetOwner<UObject>());
		const bool bIsDelegate = Property->IsA(FMulticastDelegateProperty::StaticClass());
		const bool bIsExposedToSpawn = UEdGraphSchema_K2::IsPropertyExposedOnSpawn(Property);
		const bool bIsSettableExternally = !Property->HasAnyPropertyFlags(CPF_DisableEditOnInstance);

		if (bIsExposedToSpawn &&
			!Property->HasAnyPropertyFlags(CPF_Parm) &&
			bIsSettableExternally &&
			Property->HasAllPropertyFlags(CPF_BlueprintVisible) &&
			!bIsDelegate &&
			(nullptr == FindPin(Property->GetFName())) &&
			FBlueprintEditorUtils::PropertyStillExists(Property))
		{
			if (UEdGraphPin* Pin = CreatePin(EGPD_Input, NAME_None, Property->GetFName()))
			{
				K2Schema->ConvertPropertyToPinType(Property, /*out*/ Pin->PinType);
				if (OutClassPins)
				{
					OutClassPins->Add(Pin);
				}

				if (ClassDefaultObject && K2Schema->PinDefaultValueIsEditable(*Pin))
				{
					FString DefaultValueAsString;
					const bool bDefaultValueSet = FBlueprintEditorUtils::PropertyValueToString(Property, reinterpret_cast<const uint8*>(ClassDefaultObject), DefaultValueAsString, this);
					check(bDefaultValueSet);
					K2Schema->SetPinAutogeneratedDefaultValue(Pin, DefaultValueAsString);
				}

				// Copy tooltip from the property.
				K2Schema->ConstructBasicPinTooltip(*Pin, Property->GetToolTipText(), Pin->PinToolTip);
			}
		}
	}

	//更改返回值的type
	UEdGraphPin* ResultPin = GetResultPin();
	ResultPin->PinType.PinSubCategoryObject = InClass->GetAuthoritativeClass();

	//创建delegate pin
	CreateDelegatePins(InClass);
}

void UK2Node_RunBPAbilityTask::CreateDelegatePins(UClass* InClass)
{
	check(InClass);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	//广播输出pin
	UFunction* DelegateSignatureFunction = nullptr;
	for (TFieldIterator<FProperty> PropertyIt(InClass); PropertyIt; ++PropertyIt)
	{
		if (FMulticastDelegateProperty* Property = CastField<FMulticastDelegateProperty>(*PropertyIt))
		{
			UEdGraphPin* ExecPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, Property->GetFName());
			ExecPin->PinToolTip = Property->GetToolTipText().ToString();
			ExecPin->PinFriendlyName = Property->GetDisplayNameText();

			if (!DelegateSignatureFunction)
			{
				DelegateSignatureFunction = Property->SignatureFunction;

				//参数要么没有，要么得是一样的
				for (TFieldIterator<FProperty> PropIt(DelegateSignatureFunction); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
				{
					FProperty* Param = *PropIt;
					const bool bIsFunctionInput = !Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm);
					if (bIsFunctionInput)
					{
					}
				}
			}
		}
	}
	

	//广播的参数
	if (DelegateSignatureFunction)
	{
		for (TFieldIterator<FProperty> PropIt(DelegateSignatureFunction); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
		{
			FProperty* Param = *PropIt;
			const bool bIsFunctionInput = !Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm);
			if (bIsFunctionInput)
			{
				UEdGraphPin* Pin = CreatePin(EGPD_Output, NAME_None, Param->GetFName());
				K2Schema->ConvertPropertyToPinType(Param, /*out*/ Pin->PinType);

				Pin->PinToolTip = Param->GetToolTipText().ToString();
			}
		}
	}
}

UClass* UK2Node_RunBPAbilityTask::GetTaskToCreate(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	UClass* UseCreateTask = nullptr;
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* ClassPin = GetClassPin(PinsToSearch);
	if (ClassPin && ClassPin->DefaultObject && ClassPin->LinkedTo.Num() == 0)
	{
		UseCreateTask = CastChecked<UClass>(ClassPin->DefaultObject);
	}
	else if (ClassPin && ClassPin->LinkedTo.Num())
	{
		UEdGraphPin* ClassSource = ClassPin->LinkedTo[0];
		UseCreateTask = ClassSource ? Cast<UClass>(ClassSource->PinType.PinSubCategoryObject.Get()) : nullptr;
	}

	return UseCreateTask;
}

void UK2Node_RunBPAbilityTask::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();
	if (UClass* UseSpawnClass = GetTaskToCreate(&OldPins))
	{
		CreatePinsForClass(UseSpawnClass);
	}
	RestoreSplitPins(OldPins);
}

void UK2Node_RunBPAbilityTask::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	if (UClass* UseSpawnClass = GetTaskToCreate())
	{
		CreatePinsForClass(UseSpawnClass);
	}
}


void UK2Node_RunBPAbilityTask::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	bool bIsErrorFree = true;

	UClass* TaskClassToCreate =  GetTaskToCreate();
	if (!TaskClassToCreate)
	{
		FText ErrorMessage = FText::FromString(TEXT("RunBPAbilityTask::ExpandNode: Cannot find TaskClass"));
		CompilerContext.MessageLog.Error(*ErrorMessage.ToString(), this);
		return;
	}

	//Create task Node
	UK2Node_CallFunction* CreateTaskFunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CreateTaskFunctionNode->FunctionReference.SetExternalMember(ProxyFactoryFunctionName, ProxyFactoryClass);
	CreateTaskFunctionNode->AllocateDefaultPins();
	
	//exec pin
	UEdGraphPin* NodeExecPin = GetExecPin();
	UEdGraphPin* CreateExecPin = CreateTaskFunctionNode->GetExecPin();
	CompilerContext.MovePinLinksToIntermediate(*NodeExecPin, *CreateExecPin);

	//class pin
	UEdGraphPin* NodeTaskClassPin = FindPinChecked(TaskClassPinName);
	UEdGraphPin* CreateClassPin = CreateTaskFunctionNode->FindPinChecked(TEXT("TaskClass"));
	CompilerContext.MovePinLinksToIntermediate(*NodeTaskClassPin, *CreateClassPin);

	//根据当前蓝图的类型生成task的OwnerAbility
	{
		UBlueprint* MyBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(SourceGraph);
		if (!MyBlueprint || !MyBlueprint->GeneratedClass)
		{
			FText ErrorMessage = FText::FromString(TEXT("RunBPAbilityTask: Cannot find this blueprint "));
			CompilerContext.MessageLog.Error(*ErrorMessage.ToString(), this);
			return;
		}

		UK2Node_Self* ParentSelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
		ParentSelfNode->AllocateDefaultPins();

		UEdGraphPin* ParentGraphSelfPin = ParentSelfNode->FindPinChecked(Schema->PN_Self);
		UEdGraphPin* CreateOwnerAbilityPin = CreateTaskFunctionNode->FindPinChecked(TEXT("OwnerAbility"));

		//父图是Task， 需要从task转为ability
		if (MyBlueprint->GeneratedClass->IsChildOf(UAbilityTask::StaticClass()))
		{
			// 从task中获取ability的节点
			UK2Node_CallFunction* GetAbilityNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
//			const FName GetAbilityNodeFuncName = GET_FUNCTION_NAME_CHECKED(UMOAbilityBPLibrary, GetAbilityFromTask);
//			GetAbilityNode->FunctionReference.SetExternalMember(GetAbilityNodeFuncName, UMOAbilityBPLibrary::StaticClass());
			GetAbilityNode->AllocateDefaultPins();
			UEdGraphPin* InputTaskPin = GetAbilityNode->FindPinChecked(TEXT("Task"));

			//连接节点
			Schema->TryCreateConnection(ParentGraphSelfPin, InputTaskPin);
			Schema->TryCreateConnection(GetAbilityNode->GetReturnValuePin(), CreateOwnerAbilityPin);
		}
		//父图是Ability
		else
		{
			Schema->TryCreateConnection(ParentGraphSelfPin, CreateOwnerAbilityPin);
		}
	}

	//instance name
	UEdGraphPin* NodeInstanceNamePin = FindPinChecked(TaskNamePinName, EGPD_Input);
	UEdGraphPin* CreateInstanceNamePin = CreateTaskFunctionNode->FindPinChecked(TEXT("InstanceName"));
	CompilerContext.MovePinLinksToIntermediate(*NodeInstanceNamePin, *CreateInstanceNamePin);

	UEdGraphPin* CreateTaskReturnPin = CreateTaskFunctionNode->GetReturnValuePin();
	CreateTaskReturnPin->PinType.PinSubCategoryObject = TaskClassToCreate->GetAuthoritativeClass();

	UEdGraphPin* CreateTaskThenPin = CreateTaskFunctionNode->GetThenPin();

	UEdGraphPin* LastThen = GenerateAssignmentNodes(CompilerContext, SourceGraph, CreateTaskFunctionNode, this, CreateTaskReturnPin, TaskClassToCreate);
	
	//delegate 相关的pin
	{
		TMap<FName, UK2Node_TemporaryVariable*> OutInfos;
		TArray<FName> ExcludeOutputDataPinNames;
		ExcludeOutputDataPinNames.Add(TaskResultPinName);
		HandleDelegateOutputPins(CompilerContext, OutInfos, &ExcludeOutputDataPinNames);

		for (TFieldIterator<FMulticastDelegateProperty> PropertyIt(TaskClassToCreate); PropertyIt && bIsErrorFree; ++PropertyIt)
		{
			//UK2Node_BaseAsyncTask::FBaseAsyncTaskHelper::HandleDelegateImplementation(*PropertyIt, VariableOutputs, CreateTaskReturnPin, LastThen, this, SourceGraph, CompilerContext);
			bIsErrorFree &= HandleDelegateImplementation(*PropertyIt, CreateTaskReturnPin, LastThen, OutInfos, SourceGraph, CompilerContext);
		}
	}

	//Activate Node
	{
		UK2Node_CallFunction* const CallActivateProxyObjectNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		CallActivateProxyObjectNode->FunctionReference.SetExternalMember(ProxyActivateFunctionName, TaskClassToCreate);
		CallActivateProxyObjectNode->AllocateDefaultPins();

		// Hook up the self connection
		UEdGraphPin* ActivateCallSelfPin = Schema->FindSelfPin(*CallActivateProxyObjectNode, EGPD_Input);
		check(ActivateCallSelfPin);

		Schema->TryCreateConnection(CreateTaskReturnPin, ActivateCallSelfPin);

		//lastThen -> activate 
		UEdGraphPin* ActivateExecPin = CallActivateProxyObjectNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute);
		Schema->TryCreateConnection(LastThen, ActivateExecPin);

		//last then = activate then
		LastThen = CallActivateProxyObjectNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);
	}

	//last then 为最终返回
	UEdGraphPin* NodeThenPin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	CompilerContext.MovePinLinksToIntermediate(*NodeThenPin, *LastThen);

	//return task instance
	UEdGraphPin* NodeReturnTaskPin = GetResultPin();
	CompilerContext.MovePinLinksToIntermediate(*NodeReturnTaskPin, *CreateTaskReturnPin);
}

bool UK2Node_RunBPAbilityTask::HandleDelegateOutputPins
(
	FKismetCompilerContext& CompilerContext,
	TMap<FName, UK2Node_TemporaryVariable*>& OutInfos,
	const TArray<FName>* ExcluedPinNames
) 
{
	bool bIsErrorFree = true;

	OutInfos.Empty();
	for (UEdGraphPin* CurrentPin : Pins)
	{
		if (ExcluedPinNames && ExcluedPinNames->Find(CurrentPin->PinName) != INDEX_NONE)
		{
			continue;
		}

		if (CurrentPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec
			&& CurrentPin->Direction == EGPD_Output
			&& !CurrentPin->bOrphanedPin)
		{
			const FEdGraphPinType& PinType = CurrentPin->PinType;
			UK2Node_TemporaryVariable* TempVarOutput = CompilerContext.SpawnInternalVariable(
				this, PinType.PinCategory, PinType.PinSubCategory, PinType.PinSubCategoryObject.Get(), PinType.ContainerType, PinType.PinValueType);

			bIsErrorFree &= TempVarOutput->GetVariablePin() && CompilerContext.MovePinLinksToIntermediate(*CurrentPin, *TempVarOutput->GetVariablePin()).CanSafeConnect();

			OutInfos.Add(CurrentPin->PinName, TempVarOutput);
		}
	}

	return bIsErrorFree;
}

UEdGraphPin* UK2Node_RunBPAbilityTask::GenerateAssignmentNodes
(
	FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph,
	UK2Node_CallFunction* CallBeginSpawnNode,
	UEdGraphNode* SpawnNode,
	UEdGraphPin* CallBeginResult,
	UClass* ForClass
)
{
	static const FName ObjectParamName(TEXT("Object"));
	static const FName ValueParamName(TEXT("Value"));
	static const FName PropertyNameParamName(TEXT("PropertyName"));

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	UEdGraphPin* LastThen = CallBeginSpawnNode->GetThenPin();

	// Create 'set var by name' nodes and hook them up
	for (int32 PinIdx = 0; PinIdx < SpawnNode->Pins.Num(); PinIdx++)
	{
		// Only create 'set param by name' node if this pin is linked to something
		UEdGraphPin* OrgPin = SpawnNode->Pins[PinIdx];

		//改动：当OrgPin为Output类型时，不做修改
		if (OrgPin->Direction == EGPD_Output)
		{
			continue;
		}

		//改动：当创建出来的对象没有该变量时，不做修改
		FProperty* Property = FindFProperty<FProperty>(ForClass, OrgPin->PinName);
		if (!Property)
		{
			continue;
		}

		const bool bHasDefaultValue = !OrgPin->DefaultValue.IsEmpty() || !OrgPin->DefaultTextValue.IsEmpty() || OrgPin->DefaultObject;
		if (NULL == CallBeginSpawnNode->FindPin(OrgPin->PinName) &&
			(OrgPin->LinkedTo.Num() > 0 || bHasDefaultValue))
		{
			if (OrgPin->LinkedTo.Num() == 0)
			{
				// We don't want to generate an assignment node unless the default value 
				// differs from the value in the CDO:
				FString DefaultValueAsString;

				if (FBlueprintCompilationManager::GetDefaultValue(ForClass, Property, DefaultValueAsString))
				{
					if (Schema->DoesDefaultValueMatch(*OrgPin, DefaultValueAsString))
					{
						continue;
					}
				}
				else if (ForClass->ClassDefaultObject)
				{
					FBlueprintEditorUtils::PropertyValueToString(Property, (uint8*)ForClass->ClassDefaultObject, DefaultValueAsString);

					if (DefaultValueAsString == OrgPin->GetDefaultAsString())
					{
						continue;
					}
				}
			}

			UFunction* SetByNameFunction = Schema->FindSetVariableByNameFunction(OrgPin->PinType);
			if (SetByNameFunction)
			{
				UK2Node_CallFunction* SetVarNode = nullptr;
				if (OrgPin->PinType.IsArray())
				{
					SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallArrayFunction>(SpawnNode, SourceGraph);
				}
				else
				{
					SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode, SourceGraph);
				}
				SetVarNode->SetFromFunction(SetByNameFunction);
				SetVarNode->AllocateDefaultPins();

				// Connect this node into the exec chain
				Schema->TryCreateConnection(LastThen, SetVarNode->GetExecPin());
				LastThen = SetVarNode->GetThenPin();

				// Connect the new actor to the 'object' pin
				UEdGraphPin* ObjectPin = SetVarNode->FindPinChecked(ObjectParamName);
				CallBeginResult->MakeLinkTo(ObjectPin);

				// Fill in literal for 'property name' pin - name of pin is property name
				UEdGraphPin* PropertyNamePin = SetVarNode->FindPinChecked(PropertyNameParamName);
				PropertyNamePin->DefaultValue = OrgPin->PinName.ToString();

				UEdGraphPin* ValuePin = SetVarNode->FindPinChecked(ValueParamName);
				if (OrgPin->LinkedTo.Num() == 0 &&
					OrgPin->DefaultValue != FString() &&
					OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
					OrgPin->PinType.PinSubCategoryObject.IsValid() &&
					OrgPin->PinType.PinSubCategoryObject->IsA<UEnum>())
				{
					// Pin is an enum, we need to alias the enum value to an int:
					UK2Node_EnumLiteral* EnumLiteralNode = CompilerContext.SpawnIntermediateNode<UK2Node_EnumLiteral>(SpawnNode, SourceGraph);
					EnumLiteralNode->Enum = CastChecked<UEnum>(OrgPin->PinType.PinSubCategoryObject.Get());
					EnumLiteralNode->AllocateDefaultPins();
					EnumLiteralNode->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(ValuePin);

					UEdGraphPin* InPin = EnumLiteralNode->FindPinChecked(UK2Node_EnumLiteral::GetEnumInputPinName());
					check(InPin);
					InPin->DefaultValue = OrgPin->DefaultValue;
				}
				else
				{
					// For non-array struct pins that are not linked, transfer the pin type so that the node will expand an auto-ref that will assign the value by-ref.
					if (OrgPin->PinType.IsArray() == false && OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && OrgPin->LinkedTo.Num() == 0)
					{
						ValuePin->PinType.PinCategory = OrgPin->PinType.PinCategory;
						ValuePin->PinType.PinSubCategory = OrgPin->PinType.PinSubCategory;
						ValuePin->PinType.PinSubCategoryObject = OrgPin->PinType.PinSubCategoryObject;
						CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
					}
					else
					{
						CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
						SetVarNode->PinConnectionListChanged(ValuePin);
					}

				}
			}
		}
	}

	return LastThen;
}