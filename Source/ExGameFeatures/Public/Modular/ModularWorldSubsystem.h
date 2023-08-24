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
	virtual bool NativeCanActivate(UObject* Outer);

private:
	/**
	 * @brief Modular类型的Subsystem是不能默认创建的
	*/
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Initialize")
		void ReceiveInitialize();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Deinitialize")
		void ReceiveDeinitialize();

	UFUNCTION(BlueprintNativeEvent, DisplayName = "CanActivate")
		bool CanActivate(UObject* Outer);

private:
	UPROPERTY(Transient)
	bool bShouldCreate;
};