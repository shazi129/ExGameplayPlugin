#include "ReplicatedSubObject.h"

UReplicatedSubObject::UReplicatedSubObject()
{
}

bool UReplicatedSubObject::IsNameStableForNetworking() const
{
	return Super::IsNameStableForNetworking();
}

bool UReplicatedSubObject::IsSupportedForNetworking() const
{
	return true;
}

void UReplicatedSubObject::PreNetReceive()
{
}

void UReplicatedSubObject::PostNetReceive()
{
}

