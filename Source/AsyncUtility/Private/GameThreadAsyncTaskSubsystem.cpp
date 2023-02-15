#include "GameThreadAsyncTaskSubsystem.h"

void UGameThreadAsyncTaskSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UGameThreadAsyncTaskSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TStatId UGameThreadAsyncTaskSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(GameThreadAsyncTaskSubsystem, STATGROUP_Tickables);
}

int64 UGameThreadAsyncTaskSubsystem::AddDelegateTask(const FString& GroupName, FGameThreadAsyncTaskDelegate Delegate)
{
	FGameThreadAsyncTaskGroup& TaskGroup = TaskGroups.FindOrAdd(GroupName);
	FGameThreadAsyncDelegateTask* TaskPtr = new FGameThreadAsyncDelegateTask(Delegate);
	TaskPtr->TaskID = (int64)TaskPtr;
	TaskGroup.Tasks.Add(TaskPtr);

	return TaskPtr->TaskID;
}

void UGameThreadAsyncTaskSubsystem::Tick(float DeltaTime)
{

}