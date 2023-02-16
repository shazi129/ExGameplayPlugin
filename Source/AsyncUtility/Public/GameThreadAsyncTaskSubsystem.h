#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameThreadAsyncTaskSubsystem.generated.h"

class FGameThreadAsyncTask
{
public:
	int64 TaskID;
	virtual void ExecuteTask() {};
};

DECLARE_DYNAMIC_DELEGATE(FGameThreadAsyncTaskDelegate);

class FGameThreadAsyncDelegateTask: public FGameThreadAsyncTask
{
public:
	FGameThreadAsyncTaskDelegate TaskDelegate;

	FGameThreadAsyncDelegateTask(FGameThreadAsyncTaskDelegate Delegate)
	{
		TaskDelegate = Delegate;
	}

	virtual void ExecuteTask()
	{
		if (TaskDelegate.IsBound())
		{
			TaskDelegate.Execute();
		}
	}
};

USTRUCT(BlueprintType)
struct FGameThreadAsyncTaskGroup
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName GroupName;

	TArray<FGameThreadAsyncTask*> Tasks;
};

UCLASS(BlueprintType)
class ASYNCUTILITY_API UGameThreadAsyncTaskSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual bool IsTickable() const;
	virtual void Tick(float DeltaTime);
	virtual TStatId GetStatId() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UGameThreadAsyncTaskSubsystem* GetSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	int64 AddDelegateTask(const FName& GroupName, FGameThreadAsyncTaskDelegate Delegate);

	UFUNCTION(BlueprintCallable)
	FGameThreadAsyncTaskGroup& FindOrAddGroup(const FName& GroupName);

private:
	TArray<FGameThreadAsyncTaskGroup> TaskGroups;
};