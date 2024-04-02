#include "Subsystems/GameplayAsyncTaskSubsystem.h"
#include "Macros/SubsystemMacros.h"
#include "GameplayUtilsModule.h"

std::atomic<int32> UGameplayAsyncTaskSubsystem::TaskIdGenerator = 0;


void FDelegateTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	TickDelegate.ExecuteIfBound(DeltaTime);
}

UGameplayAsyncTaskSubsystem* UGameplayAsyncTaskSubsystem::Get(const UObject* WorldContextObject)
{
	GET_WORLD_SUBSYSTEM(LogGameplayUtils, UGameplayAsyncTaskSubsystem, WorldContextObject);
}

bool UGameplayAsyncTaskSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Outer)
	{
		return false;
	}

	UWorld* World = Outer->GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

void UGameplayAsyncTaskSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UGameplayAsyncTaskSubsystem::Deinitialize()
{
	Super::Deinitialize();

	for (auto& FunctionItem : DelegateTickFunctionMap)
	{
		if (FunctionItem.Value)
		{
			FunctionItem.Value->UnRegisterTickFunction();
			FMemory::Free(FunctionItem.Value);
		}
	}

	DelegateTickFunctionMap.Empty();
}


FName UGameplayAsyncTaskSubsystem::CreateAndRegisterTickFunction(const UObject* ContextObject, const FName& Name, ETickingGroup TickGroup)
{
	if (!ContextObject)
	{
		return NAME_None;
	}

	//已经存在这个tick
	if (!Name.IsNone() && DelegateTickFunctionMap.Contains(Name))
	{
		return NAME_None;
	}

	if (UWorld* World = ContextObject->GetWorld())
	{
		//申请内存
		UScriptStruct* FunctionStruct = FDelegateTickFunction::StaticStruct();
		const int32 MinAlignment = FunctionStruct->GetMinAlignment();
		const int32 RequiredSize = FunctionStruct->GetStructureSize();
		FDelegateTickFunction* FunctionPtr = ((FDelegateTickFunction*)FMemory::Malloc(FMath::Max(1, RequiredSize), MinAlignment));
		FunctionStruct->InitializeStruct(FunctionPtr);

		//初始化
		FunctionPtr->FunctionId = ++TaskIdGenerator;
		FunctionPtr->FunctionName = Name.IsNone() ? FName(FString::Printf(TEXT("TickFunc_%d"), FunctionPtr->FunctionId)) : Name;
		FunctionPtr->bCanEverTick = true;
		FunctionPtr->TickGroup = TickGroup;
		FunctionPtr->bStartWithTickEnabled = true;
		FunctionPtr->SetTickFunctionEnable(true);
		FunctionPtr->RegisterTickFunction(World->PersistentLevel);
		FunctionPtr->ContextObject = ContextObject;

		DelegateTickFunctionMap.Add(FunctionPtr->FunctionName, FunctionPtr);

		return FunctionPtr->FunctionName;
	}
	return NAME_None;
}

FDelegateTickFunction* UGameplayAsyncTaskSubsystem::GetTickFunction(const FName& Name)
{
	if (auto FunctionMapValue = DelegateTickFunctionMap.Find(Name))
	{
		return *FunctionMapValue;
	}
	return nullptr;
}


FName UGameplayAsyncTaskSubsystem::K2_CreateAndRegisterTickFunction(const UObject* WorldContextObject, const FName& Name, ETickingGroup TickGroup, const FGameplayTickDelegate& Delegate)
{
	FName TickName = CreateAndRegisterTickFunction(WorldContextObject, Name, TickGroup);
	if (auto FunctionMapValue = DelegateTickFunctionMap.Find(TickName))
	{
		FDelegateTickFunction* FunctionPtr = *FunctionMapValue;
		FunctionPtr->TickDelegate = Delegate;
	}
	return TickName;
}

void UGameplayAsyncTaskSubsystem::RemoveTickFunction(FName Name)
{
	if (auto FunctionMapValue = DelegateTickFunctionMap.Find(Name))
	{
		FDelegateTickFunction* FunctionPtr = *FunctionMapValue;

		FunctionPtr->UnRegisterTickFunction();
		FMemory::Free(FunctionPtr);
		DelegateTickFunctionMap.Remove(Name);
	}
}


