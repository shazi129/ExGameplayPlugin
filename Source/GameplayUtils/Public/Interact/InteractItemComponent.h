#pragma once

#include "Interact/InteractItemInterface.h"
#include "Components/ActorComponent.h"
#include "InteractItemComponent.generated.h"


UCLASS(ClassGroup = (Interact), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYUTILS_API UInteractItemComponent : public UActorComponent, public IInteractItemInterface
{
	GENERATED_BODY()

public:
	UInteractItemComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetEnable(bool Enable, const FName& ConfigName);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Config", meta = (DisplayThumbnail = "false"))
	TArray<TSoftObjectPtr<UInteractConfigAsset>> InteractConfigAssets;

	//特殊的状态变化处理
	UPROPERTY(EditAnywhere, Category = "Interact Config")
	TMap<FString, TSoftClassPtr<UInteractItemHandler>> StateChangeHandlerConfig;

	//特殊的交互处理
	UPROPERTY(EditAnywhere, Category = "Interact Config")
	TMap<FString, TSoftClassPtr<UInteractItemHandler>> InteractHandlerConfig;

	UPROPERTY(BlueprintAssignable)
	FOnInteractStateChange InteractStateChangeDelegate;

	UPROPERTY(BlueprintReadWrite)
	FCanInteract CanInteractDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInteract StartInteractDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInteract EndInteractDelegate;

	virtual void OnInteractStateChange_Implementation(const FInteractInstanceData& InteractData) override;
	virtual bool CanInteract_Implementation(const FInteractInstanceData& InteractData) override;
	virtual void StartInteract_Implementation(const FInteractInstanceData& InteractData) override;
	virtual void EndInteract_Implementation(const FInteractInstanceData& InteractData) override;


	virtual int32 GetInteractingNum_Implementation(FName const& ConfigName);
	virtual void AddInteractingPawn_Implementation(FName const& ConfigName, APawn* Pawn);
	virtual void RemoveInteractingPawn_Implementation(const FName& ConfigName, APawn* Pawn);


	virtual TArray<FInteractConfigData> GetInteractConfigs_Implementation();

	virtual UInteractItemHandler* GetStateChangeHandler(const FName& ConfigName);
	virtual UInteractItemHandler* GetInteractHandler(const FName& ConfigName);

public:
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<FInteractReplicateData> InteractingInfoList;

	UPROPERTY()
	TMap<FString, UInteractItemHandler*> StateChangeHandlerMap;

	UPROPERTY()
	TMap<FString, UInteractItemHandler*> InteractHandlerMap;

	UPROPERTY()
	TMap<FString, UInteractConfigAsset*> InteractAssetMap;
};