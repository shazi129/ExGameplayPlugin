#pragma once


#include "Components/ActorComponent.h"
#include "Props/PropsItemComponent.h"
#include "PropsManagerComponent.generated.h"

UCLASS(ClassGroup = (Props), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UPropsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void PickupPropsItem(UPropsItemComponent* PropsItemComponent);

	UFUNCTION(BlueprintCallable)
	void DiscardPropsItem(int PropsType, int PropsID, int PropsCount);
	
private:
	TArray<FPropsItemInfo> PropsItemList;
};