#pragma once

#include "Actions/GameFeatureAction_GameInstanceActionBase.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction_AddActorComponents.generated.h"

USTRUCT()
struct EXGAMEFEATURES_API FAddActorComponentEntry
{
	GENERATED_BODY()

	// The base actor class to add a component to
	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowAbstract = "True"))
		TSoftClassPtr<AActor> ActorClass;

	// The component class to add to the specified type of actor
	UPROPERTY(EditAnywhere, Category = "Components")
		TSoftClassPtr<UActorComponent> ComponentClass;

	// Should this component be added for clients
	UPROPERTY(EditAnywhere, Category = "Components")
		uint8 bClientComponent : 1;

	// Should this component be added on servers
	UPROPERTY(EditAnywhere, Category = "Components")
		uint8 bServerComponent : 1;

	FAddActorComponentEntry()
		: bClientComponent(true)
		, bServerComponent(true)
	{
	}
};

UCLASS(MinimalAPI, meta = (DisplayName = "Add Actor Components"))
class UGameFeatureAction_AddActorComponents final : public UGameFeatureAction_GameInstanceActionBase
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
#endif
	//~End of UGameFeatureAction interface

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif
	//~End of UObject interface

	/** List of components to add to gameplay actors when this game feature is enabled */
	UPROPERTY(EditAnywhere, Category="Components", meta=(TitleProperty="{ActorClass} -> {ComponentClass}"))
	TArray<FAddActorComponentEntry> ComponentList;

protected:
	virtual bool AddToGameInstance(UGameInstance* GameInstance) override;
private:

	TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequestHandles;
};
