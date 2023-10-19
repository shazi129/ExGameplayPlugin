#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PawnStateCheatExtension.generated.h"

UCLASS()
class PAWNSTATE_API UPawnStateCheatExtension : public UCheatManagerExtension
{
	GENERATED_BODY()

public:

	UPawnStateCheatExtension();

	UFUNCTION(Exec)
	void PawnState(const FString& Param);

	static FString GetPawnStateDebugString(class UPawnStateComponent* Component);
	static FString GetASCTagsDebugString(AActor* Actor);

private:
	void ShowPawnStateUsage();
	void ShowPawnState();
	void ShowASCTags();
	void ShowConfig();

	void ShowPawnStateServer();
	void ShowASCTagsServer();

	UFUNCTION()
	void OnMessageReceived(const FGameplayMessage& Message);

private:
	bool bRegisterGetServerState;
	bool bRegisterGetServerTag;
};