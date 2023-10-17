#include "FunctionLibraries/MassHelperLibrary.h"
#include "MassEntitySubsystem.h"
#include "GameplayUtilsModule.h"
#include "MassExecutor.h"

void UMassHelperLibrary::ExecuteProcessor(const UObject* ContextObject, UMassProcessor* Processor, const FInstancedStruct& Data)
{
	if (!ContextObject)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, ContextObject is null"), *FString(__FUNCTION__));
		return;
	}

	UMassEntitySubsystem* MassEntitySubsystem = Processor->GetWorld()->GetSubsystem<UMassEntitySubsystem>();
	if (!MassEntitySubsystem)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, Cannot get UMassEntitySubsystem"), *FString(__FUNCTION__));
		return;
	}

	FMassEntityManager& EntityManager = MassEntitySubsystem->GetMutableEntityManager();
	FMassProcessingContext ProcessingContext(EntityManager, 0.0f);
	ProcessingContext.AuxData = Data;
	ProcessingContext.bFlushCommandBuffer = false;

	UE::Mass::Executor::Run(*Processor, ProcessingContext);
}
