#include "Modular/ModularWorldSubsystem.h"

UModularWorldSubsystem::UModularWorldSubsystem()
{
	bShouldCreate = false;
}

void UModularWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReceiveInitialize();
}

void UModularWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	ReceiveDeinitialize();
}

bool UModularWorldSubsystem::NativeCanActivate(UObject* Outer)
{
	return CanActivate(Outer);
}

bool UModularWorldSubsystem::CanActivate_Implementation(UObject* Outer)
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