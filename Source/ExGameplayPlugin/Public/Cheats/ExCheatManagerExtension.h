#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "ExCheatManagerExtension.generated.h"

UCLASS()
class UExCheatManagerExtension : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		static void LogAndCopyToClipboard(const FString Value);

	UFUNCTION(Exec)
		void ExCheat(const FString& Param);

#pragma region WorldComposition相关Cheat
public:
	UFUNCTION(Exec)
	void WC(const FString& Param);

private:
	void ShowWCUsage();
	void ShowLevelInfo();
#pragma endregion

#pragma region Character相关的Cheat
public:
	UFUNCTION(Exec)
		void Character(const FString& Param);

private:
	void ShowCharacterUsage(ACharacter* Character);
	void SetSyncCharacterMovement(ACharacter* Character, bool Sync);
#pragma endregion

#pragma region 场景相关Cheat
public:
	UFUNCTION(Exec)
		void Scene(const FString& Param);

private:
	void ShowSceneUsage();
	void ShowActorWithName(const FString& ActorName);
	void DestroyActorByName(const FString& ActorName);
#pragma endregion

private:

	void ShowActorComponets(AActor* Actor);
	
};