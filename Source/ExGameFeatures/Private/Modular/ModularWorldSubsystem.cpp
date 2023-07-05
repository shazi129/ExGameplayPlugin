#include "Modular/ModularWorldSubsystem.h"

UModularWorldSubsystem::UModularWorldSubsystem()
{

}

bool UModularWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return false;
}