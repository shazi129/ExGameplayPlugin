#include "Modular/ModularWorldSubsystem.h"

UModularWorldSubsystem::UModularWorldSubsystem()
    : UWorldSubsystem()
    , bReady(false)
{

}

bool UModularWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (bReady)
    {
        return Super::ShouldCreateSubsystem(Outer);
    }
    return false;
}
