#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeatureAction_RemoveComponent.generated.h"

USTRUCT(BlueprintType)
struct FRedundantComponentEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Component")
		TSoftClassPtr<AActor> OwnerActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FName> ComponentTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 bInDedicateServer: 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 bInClient: 1;
};

UCLASS(MinimalAPI, meta = (DisplayName = "Remove Actor Component"))
class UGameFeatureAction_RemoveComponent final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
#endif
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif
	//~ End UObject interface

	void OnOwnerActorAdded(AActor* OwnerActor, FName AddTag);

	void RemoveComponents(AActor* OwnerActor);

	UPROPERTY(EditAnywhere)
		TArray<FRedundantComponentEntry> ComponentList;

private:
	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequestHandles;
};
