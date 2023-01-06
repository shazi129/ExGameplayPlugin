#include "PawnState/PawnStateSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/ExGameplayPluginSettings.h"
#include "PawnState/PawnStateComponent.h"
#include "GameFramework/Character.h"

UPawnStateSubsystem* UPawnStateSubsystem::GetPawnStateSubsystem(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UPawnStateSubsystem>();
}

UPawnStateComponent* UPawnStateSubsystem::GetPawnStateComponent(AActor* Actor)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}
	UPawnStateComponent* Component = Cast<UPawnStateComponent>(Actor->GetComponentByClass(UPawnStateComponent::StaticClass()));
	return Component;
}

UPawnStateComponent* UPawnStateSubsystem::GetLocalPawnStateComponent(const UObject* WorldContextObject)
{
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0))
	{
		return  UPawnStateSubsystem::GetPawnStateComponent(Character);
	}
	return nullptr;
}

bool UPawnStateSubsystem::CanEnterState(AActor* Actor, const FGameplayTag& NewPawnState)
{
	if (UPawnStateComponent* Component = UPawnStateSubsystem::GetPawnStateComponent(Actor))
	{
		return Component->CanEnterState(NewPawnState);
	}
	return false;
}

bool UPawnStateSubsystem::EnterState(AActor* Actor, const FGameplayTag& NewPawnState)
{
	if (UPawnStateComponent* Component = UPawnStateSubsystem::GetPawnStateComponent(Actor))
	{
		return Component->EnterState(NewPawnState);
	}
	return false;
}

bool UPawnStateSubsystem::LeaveState(AActor* Actor, const FGameplayTag& PawnState)
{
	if (UPawnStateComponent* Component = UPawnStateSubsystem::UPawnStateSubsystem::GetPawnStateComponent(Actor))
	{
		return Component->LeaveState(PawnState);
	}
	return false;
}

void UPawnStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadDefaultConfig();
}

void UPawnStateSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPawnStateSubsystem::LoadDefaultConfig()
{
	PawnStateConfig.Empty();

	const TArray<FPawnStateConfigItem>& DefaultConfig = GetMutableDefault<UExGameplayPluginSettings>()->PawnStateConfig;
	for (const FPawnStateConfigItem& ConfigItem : DefaultConfig)
	{
		FPawnStateMapCondition& MapCondition = PawnStateConfig.FindOrAdd(ConfigItem.FromState);
		for (const FPawnStateCondition& Condition : ConfigItem.Conditions)
		{
			if (MapCondition.Data.Contains(Condition.ToState))
			{
				MapCondition.Data[Condition.ToState] = Condition.Condition;
			}
			else
			{
				MapCondition.Data.Add(Condition.ToState, Condition.Condition);
			}
		}
	}
}

EPawnStateRelation UPawnStateSubsystem::GetStateRelation(const FGameplayTag& From, const FGameplayTag& To)
{
	if (PawnStateConfig.Contains(From) && PawnStateConfig[From].Data.Contains(To))
	{
		return  PawnStateConfig[From].Data[To];
	}
	return EPawnStateRelation::E_COEXIST;
}