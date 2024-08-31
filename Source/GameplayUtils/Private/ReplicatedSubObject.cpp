#include "ReplicatedSubObject.h"
#include "Net/UnrealNetwork.h"
#include "GameplayUtilsModule.h"

UReplicatedSubObject::UReplicatedSubObject()
{
}

void UReplicatedSubObject::Initialize()
{
	ReceiveInitialize();
}

void UReplicatedSubObject::Deinitialize()
{
	ReceiveDeInitialize();
}

void UReplicatedSubObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UReplicatedSubObject, ObjectTags, COND_None);
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

bool UReplicatedSubObject::HasTag(const FString& Tag)
{
	return ObjectTags.Find(Tag) >= 0;
}

void UReplicatedSubObject::AddTag(const FString& Tag)
{
	if (AActor* Outer = Cast<AActor>(GetOuter()))
	{
		if (Outer->HasAuthority())
		{
			ObjectTags.AddUnique(Tag);
		}
	}
}

TArray<FString> UReplicatedSubObject::GetTags()
{
	return ObjectTags;
}

int32 UReplicatedSubObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if ((Function->FunctionFlags & FUNC_Static))
	{
		// Try to use the same logic as function libraries for static functions, will try to use the global context to check authority only/cosmetic
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	}

	AActor* MyOwner = Cast<AActor>(GetOuter());
	return (MyOwner ? MyOwner->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool UReplicatedSubObject::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	bool bProcessed = false;

	if (AActor* MyOwner = Cast<AActor>(GetOuter()))
	{
		FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
		if (Context != nullptr)
		{
			for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
			{
				if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(MyOwner, Function))
				{
					Driver.NetDriver->ProcessRemoteFunction(MyOwner, Function, Parameters, OutParms, Stack, this);
					bProcessed = true;
				}
			}
		}
	}

	return bProcessed;
}

void UReplicatedSubObject::PreDestroyFromReplication()
{
	Super::PreDestroyFromReplication();
	GAMEPLAYUTILS_LOG(Log, TEXT("%s"), *FString(__FUNCTION__));

	Deinitialize();
}

