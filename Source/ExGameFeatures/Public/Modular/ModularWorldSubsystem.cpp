#include "Modular/ModularWorldSubsystem.h"

bool UModularWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return Super::ShouldCreateSubsystem(Outer);
}
