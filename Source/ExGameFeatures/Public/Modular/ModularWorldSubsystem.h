#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "ModularWorldSubsystem.generated.h"

UCLASS(abstract)
class EXGAMEFEATURES_API UModularWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};