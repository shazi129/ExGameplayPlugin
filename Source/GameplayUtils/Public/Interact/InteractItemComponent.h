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
	TArray<UInteractConfigAsset*> InteractConfigAssets;

	UPROPERTY(EditAnywhere, Instanced, Category = "Interact Config")
	TMap<FString, UInteractItemHandler*> StateChangeHandlerMap;

	UPROPERTY(EditAnywhere, Instanced, Category = "Interact Config")
	TMap<FString, UInteractItemHandler*> InteractHandlerMap;

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


	virtual int32 GetInteractingNum_Implementation(FName const& ConfigName);
	virtual void AddInteractingPawn_Implementation(FName const& ConfigName, APawn* Pawn);
	virtual void RemoveInteractingPawn_Implementation(const FName& ConfigName, APawn* Pawn);


	virtual TArray<UInteractConfigAsset*> GetConfigAssets_Implementation() override {return InteractConfigAssets;}

	virtual UInteractItemHandler* GetStateChangeHandler_Implementation(const FName& ConfigName);
	virtual UInteractItemHandler* GetInteractHandler_Implementation(const FName& ConfigName);

public:
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<FInteractReplicateData> InteractingInfoList;

};