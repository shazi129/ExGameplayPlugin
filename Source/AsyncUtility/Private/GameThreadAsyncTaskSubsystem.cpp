#include "GameThreadAsyncTaskSubsystem.h"
#include "Kismet/GameplayStatics.h"

UGameThreadAsyncTaskSubsystem* UGameThreadAsyncTaskSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UGameThreadAsyncTaskSubsystem>();
}

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

FGameThreadAsyncTaskGroup& UGameThreadAsyncTaskSubsystem::FindOrAddGroup(const FName& GroupName)
{
	for (FGameThreadAsyncTaskGroup& Group : TaskGroups)
	{
		if (GroupName == Group.GroupName)
		{
			return Group;
		}
	}
	
	return *(new (TaskGroups)FGameThreadAsyncTaskGroup());
}

int64 UGameThreadAsyncTaskSubsystem::AddDelegateTask(const FName& GroupName, FGameThreadAsyncTaskDelegate Delegate)
{
	FGameThreadAsyncTaskGroup& TaskGroup = FindOrAddGroup(GroupName);
	FGameThreadAsyncDelegateTask* TaskPtr = new FGameThreadAsyncDelegateTask(Delegate);
	TaskPtr->TaskID = (int64)TaskPtr;
	TaskGroup.Tasks.Add(TaskPtr);

	return TaskPtr->TaskID;
}

bool UGameThreadAsyncTaskSubsystem::IsTickable() const
{
	return TaskGroups.Num() > 0;
}

void UGameThreadAsyncTaskSubsystem::Tick(float DeltaTime)
{
	for (int i = TaskGroups.Num() - 1; i >= 0; i--)
	{
		if (TaskGroups[i].Tasks.Num() == 0)
		{
			TaskGroups.RemoveAt(i);
		}
	}

	double TickStartTime = FPlatformTime::Seconds();
	bool TickTimeout = false;

	for (FGameThreadAsyncTaskGroup& Group : TaskGroups)
	{
		do 
		{
			FGameThreadAsyncTask* TaskPtr = Group.Tasks[0];
			if (TaskPtr != nullptr)
			{
				TaskPtr->ExecuteTask();
			}
			Group.Tasks.RemoveAt(0);

			TickTimeout = (FPlatformTime::Seconds() - TickStartTime) >= 0.033f;
		}
		while (Group.Tasks.Num() > 0 && TickTimeout);

		if (TickTimeout)
		{
			break;
		}
	}
}