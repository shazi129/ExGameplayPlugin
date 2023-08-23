#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "ModularWorldSubsystem.generated.h"

UCLASS(abstract, BlueprintType, Blueprintable)
class EXGAMEFEATURES_API UModularWorldSubsystem : public UWorldSubsystem
{
	friend class UGameFeatureAction_AddWorldSubsystem;

	GENERATED_BODY()

public:
	UModularWorldSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection);
	virtual void Deinitialize();
	virtual bool CanActivate(UObject* Outer);

private:
	/**
	 * @brief Modular类型的Subsystem是不能默认创建的
	*/
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Initialize")
		void BP_Initialize();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Deinitialize")
		void BP_Deinitialize();

	UFUNCTION(BlueprintNativeEvent, DisplayName = "CanActivate")
		bool BP_CanActivate(UObject* Outer);

private:
	UPROPERTY(Transient)
	bool bShouldCreate;
};