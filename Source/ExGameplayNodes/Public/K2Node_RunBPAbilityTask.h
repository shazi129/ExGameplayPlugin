#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Textures/SlateIcon.h"
#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_RunBPAbilityTask.generated.h"

class UEdGraph;

UCLASS()
class UK2Node_RunBPAbilityTask : public UK2Node
{
	GENERATED_BODY()

public:
	UK2Node_RunBPAbilityTask(const FObjectInitializer& ObjectInitializer);

 	//~ Begin UEdGraphNode Interface.
 	virtual void AllocateDefaultPins() override;
	virtual bool IsCompatibleWithGraph(UEdGraph const* TargetGraph) const override;
 	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
 	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
 	virtual FText GetTooltipText() const override;
	virtual void PostPlacedNewNode() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin);
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
 	//~ End UEdGraphNode Interface.
 
 	//~ Begin UK2Node Interface
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
 	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
 	//~ End UK2Node Interface

public:
	//抄自 FKismetCompilerUtilities::GenerateAssignmentNodes， 这个函数有点bug，为了不改引擎，重写一遍
	//原函数bug：主节点创建了一个返回值Pin，
	//            它的名字和不存在实际创建的节点中，
	//            那么这个函数会尝试给这个Pin赋值，那么这个Pin的类型会从Output变为Input
	static UEdGraphPin* GenerateAssignmentNodes
	(
		FKismetCompilerContext& CompilerContext,
		UEdGraph* SourceGraph,
		UK2Node_CallFunction* CallBeginSpawnNode,
		UEdGraphNode* SpawnNode,
		UEdGraphPin* CallBeginResult,
		UClass* ForClass
	);

protected:
	virtual UClass* GetClassPinBaseClass() const;

	virtual UFunction* GetFactoryFunction() const;

	//动态创建bp task的一些pin
	virtual void CreatePinsForClass(UClass* InClass, TArray<UEdGraphPin*>* OutClassPins = nullptr);

	//创建Delegate的pin
	virtual void CreateDelegatePins(UClass* InClass);

	//输入Task class 
	virtual UEdGraphPin* GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const;
	/** Refresh pins when class was changed */
	virtual void OnClassPinChanged();

	virtual UEdGraphPin* GetResultPin() const;

	/** Get the task that we are going to create, if it's defined as default value */
	UClass* GetTaskToCreate(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const;

	//随着task class 不同而不同的pin
	virtual bool IsSpawnVarPin(UEdGraphPin* Pin) const;

	/*
	 * 对delegate输出pin做处理, 主要是为这些pin分配一个UK2Node_TemporaryVariable，方便custom event赋值
	 * 因为这些pin是output类型的，custom envent的参数也是output类型，不能直接连接，所以需要一个UK2Node_TemporaryVariable
	 * 
	 */
	virtual bool HandleDelegateOutputPins
	(
		FKismetCompilerContext& CompilerContext,
		TMap<FName, UK2Node_TemporaryVariable*>& OutInfos,
		const TArray<FName>* ExcluedPinNames = nullptr
	);

	//实现delegate 相关逻辑
	virtual bool HandleDelegateImplementation
	(
		FMulticastDelegateProperty* CurrentProperty,
		UEdGraphPin* CreatedTaskPin,
		UEdGraphPin*& InOutLastThenPin,
		TMap<FName, UK2Node_TemporaryVariable*>& OutInfos,
		UEdGraph* SourceGraph,
		FKismetCompilerContext& CompilerContext
	);

protected:
	UPROPERTY()
		FName TaskClassPinName;
	UPROPERTY()
		FName TaskResultPinName;
	UPROPERTY()
		FName TaskNamePinName;

	UPROPERTY()
		FName ProxyFactoryFunctionName;

	UPROPERTY()
		UClass* TaskClass;

	UPROPERTY()
		UClass* ProxyFactoryClass;

	//Activat task的方法
	UPROPERTY()
		FName ProxyActivateFunctionName;
};

