#pragma once 


#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayAsyncTaskSubsystem.generated.h"

/**
 * Tick任务相关，可以在各种TickGroup中定义tick逻辑
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FGameplayTickDelegate, float, DeltaTime);

USTRUCT()
struct FDelegateTickFunction : public FTickFunction
{
	GENERATED_BODY()

	int FunctionId = 0;

	FName FunctionName;

	TWeakObjectPtr<const UObject> ContextObject;

	FGameplayTickDelegate TickDelegate;

	GAMEPLAYUTILS_API virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
};

template<>
struct TStructOpsTypeTraits<FDelegateTickFunction> : public TStructOpsTypeTraitsBase2<FDelegateTickFunction>
{
	enum
	{
		WithCopy = false
	};
};


/**
 *  分帧任务相关，可以将一个大的任务分成几帧来做, 只在主线程中运行
 */
DECLARE_DYNAMIC_DELEGATE(FFrameSplittingTaskDelegate);

USTRUCT()
struct FFrameSplittingTask
{
	GENERATED_BODY()

	//分帧任务名
	FName GroupName;

	//分帧任务执行的阶段
	ETickingGroup TickGroup;

	//分帧任务最大执行时间
	float MaxSeconds;

	//分帧任务回调
	TArray<FFrameSplittingTaskDelegate> TaskDelegates;
};



UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGameplayAsyncTaskSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UGameplayAsyncTaskSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#pragma region --------------------Tick任务----------------------------
public:
	FName CreateAndRegisterTickFunction(const UObject* ContextObject, const FName& Name, ETickingGroup TickGroup);

	FDelegateTickFunction* GetTickFunction(const FName& Name);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true", DisplayName = "CreateAndRegisterTickFunction", ScriptName = "CreateAndRegisterTickFunction"))
	FName K2_CreateAndRegisterTickFunction(const UObject* WorldContextObject, const FName& Name, ETickingGroup TickGroup, const FGameplayTickDelegate& Delegate);

	UFUNCTION(BlueprintCallable)
	void RemoveTickFunction(FName Name);

private:
	TMap<FName, FDelegateTickFunction*> DelegateTickFunctionMap;

#pragma endregion


#pragma region --------------------主线程分帧任务----------------------------

private:
	static std::atomic<int32> TaskIdGenerator;
};