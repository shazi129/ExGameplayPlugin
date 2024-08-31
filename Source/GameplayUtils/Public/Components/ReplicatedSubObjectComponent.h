#pragma once

/**
 * 
 */

#include "ReplicatedSubObject.h"
#include "Components/ActorComponent.h"
#include "ReplicatedSubObjectComponent.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UReplicatedSubObjectsAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<UReplicatedSubObject>> ObjectClassList;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UReplicatedSubObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UReplicatedSubObjectComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	UReplicatedSubObject* GetSubObjectByTag(const FString& ObjectTag);

	UFUNCTION(BlueprintCallable)
	UReplicatedSubObject* GetSubObjectByClass(UClass* Class);

	UFUNCTION(BlueprintCallable)
	UReplicatedSubObject* AddSubObjectByClass(TSubclassOf<UReplicatedSubObject> SubObjectClass);

	UFUNCTION(BlueprintCallable)
	void RemoveSubObject(UReplicatedSubObject* SubObject);

public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UReplicatedSubObjectsAsset> ReplicatedSubObjectsAsset;

protected:
	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<TObjectPtr<UReplicatedSubObject>> SubObjectList;
};