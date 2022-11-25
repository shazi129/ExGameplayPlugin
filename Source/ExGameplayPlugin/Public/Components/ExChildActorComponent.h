#pragma once 

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "ExChildActorComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXGAMEPLAYPLUGIN_API UExChildActorComponent : public UChildActorComponent
{
	GENERATED_BODY()

public:
	virtual void CreateChildActor() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool DoNotSpawnInDedicatedServer = false;
};