#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalVariableSubsystem.generated.h"

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGlobalVariableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static UGlobalVariableSubsystem* GetSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
		void SetServerTimestamp(int64 InServerTimestamp);

	UFUNCTION(BlueprintCallable)
		int64 GetServerTimestamp();

	UFUNCTION(BlueprintCallable)
		void SetIntVariable(const FName& Name, int Value);

	UFUNCTION(BlueprintCallable)
		bool GetIntVariable(const FName& Name, int& OutValue);

	UFUNCTION(BlueprintCallable)
		void SetBoolVariable(const FName& Name, bool Value);

	UFUNCTION(BlueprintCallable)
		bool GetBoolVariable(const FName& Name, bool& OutValue);

private:
	template<typename T>
	void SetVariable(TMap<FName, T>& VariableCache, const FName& Name, const T& Value)
	{
		T* ValuePtr = VariableCache.Find(Name);
		if (ValuePtr)
		{
			*ValuePtr = Value;
		}
		else
		{
			VariableCache.Add(Name, Value);
		}
	}

	template<typename T>
	bool GetVariable(TMap<FName, T>& VariableCache, const FName& Name, T& Value)
	{
		T* ValuePtr = VariableCache.Find(Name);
		if (ValuePtr)
		{
			Value = *ValuePtr;
		}
		return false;
	}

private:
	//一些常规的数据
	int64 ServerTimestamp; //服务器时间
	int64 STUpdateTime; //ServerTimestamp的更新客户端时间

private:
	TMap<FName, int32> IntVariables;
	TMap<FName, bool> BoolVariables;
};
