#include "GlobalVariableSubsystem.h"
#include "GameplayUtilsModule.h"
#include "Macros/SubsystemMacros.h"

UGlobalVariableSubsystem* UGlobalVariableSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogGameplayUtils, UGlobalVariableSubsystem, WorldContextObject);
}

void UGlobalVariableSubsystem::SetServerTimestamp(int64 InServerTimestamp)
{
}

int64 UGlobalVariableSubsystem::GetServerTimestamp()
{
	return int64();
}

void UGlobalVariableSubsystem::SetIntVariable(const FName& Name, int Value)
{
	return SetVariable(IntVariables, Name, Value);
}

bool UGlobalVariableSubsystem::GetIntVariable(const FName& Name, int& OutValue)
{
	return GetVariable(IntVariables, Name, OutValue);
}

void UGlobalVariableSubsystem::SetBoolVariable(const FName& Name, bool Value)
{
	return SetVariable(BoolVariables, Name, Value);
}

bool UGlobalVariableSubsystem::GetBoolVariable(const FName& Name, bool& OutValue)
{
	return GetVariable(BoolVariables, Name, OutValue);
}
