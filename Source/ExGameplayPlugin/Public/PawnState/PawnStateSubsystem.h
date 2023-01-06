#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PawnState/PawnStateTypes.h"
#include "PawnStateSubsystem.generated.h"

UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UPawnStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UPawnStateSubsystem* GetPawnStateSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static class UPawnStateComponent* GetLocalPawnStateComponent(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
		static class UPawnStateComponent* GetPawnStateComponent(AActor* Actor);

	
		

	UFUNCTION(BlueprintPure)
		static bool CanEnterState(AActor* Actor, const FGameplayTag& NewPawnState);

	UFUNCTION(BlueprintCallable)
		static bool EnterState(AActor* Actor, const FGameplayTag& NewPawnState);

	UFUNCTION(BlueprintCallable)
		static bool LeaveState(AActor* Actor, const FGameplayTag& PawnState);

	UFUNCTION(BlueprintPure)
		EPawnStateRelation GetStateRelation(const FGameplayTag& From, const FGameplayTag& To);

private:
	void LoadDefaultConfig();

private:
	UPROPERTY()
	TMap<FGameplayTag, FPawnStateMapCondition> PawnStateConfig;
};
