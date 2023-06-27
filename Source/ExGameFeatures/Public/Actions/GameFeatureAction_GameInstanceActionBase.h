#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_GameInstanceActionBase.generated.h"

UCLASS(Abstract)
class UGameFeatureAction_GameInstanceActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating() override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	virtual FString ToString() const;

protected:
	virtual bool AddToGameInstance(UGameInstance* GameInstance);

private:
	void HandleGameInstanceStart(UGameInstance* GameInstance);
	FDelegateHandle GameInstanceStartHandle;

	UPROPERTY()
		TArray<UGameInstance*> HandledGameInstances;
};