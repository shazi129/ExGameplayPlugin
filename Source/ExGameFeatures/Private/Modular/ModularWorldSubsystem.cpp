#include "Modular/ModularWorldSubsystem.h"

UModularWorldSubsystem::UModularWorldSubsystem()
{
	bShouldCreate = false;
}

void UModularWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	BP_Initialize();
}

void UModularWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	BP_Deinitialize();
}

bool UModularWorldSubsystem::CanActivate(UObject* Outer)
{
	return BP_CanActivate(Outer);
}

bool UModularWorldSubsystem::BP_CanActivate_Implementation(UObject* Outer)
{
	return true;
}

bool UModularWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (UWorld* World = Outer->GetWorld())
	{
		return World->IsGameWorld() && bShouldCreate;
	}
	return false;
}