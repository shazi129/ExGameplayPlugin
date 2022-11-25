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

UPawnStateComponent* UPawnStateSubsystem::GetLocalPawnStateComponent(const UObject* WorldContextObject)
{
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0))
	{
		UPawnStateComponent* Component = Cast<UPawnStateComponent>(Character->GetComponentByClass(UPawnStateComponent::StaticClass()));
		return Component;
	}

	return nullptr;
}

bool UPawnStateSubsystem::LocalCanEnterState(const UObject* WorldContextObject, const FGameplayTag& NewPawnState)
{
	if (UPawnStateComponent* Component = UPawnStateSubsystem::GetLocalPawnStateComponent(WorldContextObject))
	{
		return Component->CanEnterState(NewPawnState);
	}
	return false;
}

bool UPawnStateSubsystem::LocalEnterState(const UObject* WorldContextObject, const FGameplayTag& NewPawnState)
{
	if (UPawnStateComponent* Component = UPawnStateSubsystem::GetLocalPawnStateComponent(WorldContextObject))
	{
		return Component->EnterState(NewPawnState);
	}
	return false;
}

bool UPawnStateSubsystem::LocalLeaveState(const UObject* WorldContextObject, const FGameplayTag& PawnState)
{
	if (UPawnStateComponent* Component = UPawnStateSubsystem::GetLocalPawnStateComponent(WorldContextObject))
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

EPawnStateRelation UPawnStateSubsystem::GetRelation(const FGameplayTag& From, const FGameplayTag& To)
{
	if (PawnStateConfig.Contains(From) && PawnStateConfig[From].Data.Contains(To))
	{
		return  PawnStateConfig[From].Data[To];
	}
	return EPawnStateRelation::E_COEXIST;
}