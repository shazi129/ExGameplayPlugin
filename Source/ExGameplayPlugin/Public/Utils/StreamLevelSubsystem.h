#pragma once

#include "CoreMinimal.h"
#include "StreamLevelSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FSpawnSubLevelActorDelegate, AActor*, Actor);

USTRUCT(BlueprintType)
struct FSpawnActorDesc
{
	GENERATED_BODY()

	UWorld* World;
	UClass* ActorClass;
	FString SubLevelName;
	FSpawnSubLevelActorDelegate Delegate;
};

UCLASS(BlueprintType)
class UStreamLevelSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	bool SpawnActor(UObject* WorldContextObject, UClass* ActorClass, const FString& SubLevelName, FSpawnSubLevelActorDelegate Delegate);

	void OnLevelAdded(ULevel* Level, UWorld* World);

private:
	AActor* DoSpawnActor(UWorld* World, ULevel* Level, UClass* ActorClass, FSpawnSubLevelActorDelegate& Delegate);

private:
	FDelegateHandle LevelAddedHandle;

	TArray<FSpawnActorDesc> SpawnActorDescCache;
};