#pragma once

#include "GameplayUtilsModule.h"
#include "Macros/SubsystemMacros.h"
#include "GlobalVariableSubsystem.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGlobalVariableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_GET_GAMEINSTANCE_SUBSYSTEM(UGlobalVariableSubsystem, LogGameplayUtils)

	UFUNCTION(BlueprintCallable)
		void SetServerTimestamp(int64 InServerTimestamp);

	UFUNCTION(BlueprintCallable)
		int64 GetServerTimestamp();

	UFUNCTION(BlueprintCallable)
		void SetIntVariable(const FName& Name, int Value);

	UFUNCTION(BlueprintCallable)
		bool GetIntVariable(const FName& Name, int& OutValue);

private:
	//一些常规的数据
	int64 ServerTimestamp; //服务器时间
	int64 STUpdateTime; //ServerTimestamp的更新客户端时间

private:
	TMap<FName, int32> IntVariables;
};
