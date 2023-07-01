#include "Cheats/ExCheatManagerExtension.h"
#include "ExGameplayPluginModule.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ExCharacter.h"
#include "PlatformLibrary.h"
#include "GameFramework/ExCharacterMovementComponent.h"
#include "ExGameplayLibrary.h"

void UExCheatManagerExtension::LogAndCopyToClipboard(const FString Value)
{
	EXGAMEPLAY_LOG(Log, TEXT("%s"), *Value);
	UPlatformLibrary::ClipboardCopy(Value);
}

void UExCheatManagerExtension::ExCheat(const FString& Param)
{
	LogAndCopyToClipboard("WC Charcter");
}

#pragma region WorldComposition相关Cheat
void UExCheatManagerExtension::WC(const FString& Param)
{
	if (UKismetSystemLibrary::ParseParam(Param, "Show"))
	{
		ShowLevelInfo();
	}
	else
	{
		ShowWCUsage();
	}
}

void UExCheatManagerExtension::ShowWCUsage()
{
	FString Usage;
	Usage.Append(FString::Printf(TEXT("WC -Show")));
	LogAndCopyToClipboard(Usage);
}

void UExCheatManagerExtension::ShowLevelInfo()
{
	const TIndirectArray<FWorldContext>& WorldContextList = GEngine->GetWorldContexts();

	FString Result = FString::Printf(TEXT("ShowLevelInfo Current World Context Num: %d \n"), WorldContextList.Num());
	for (const FWorldContext& WorldContext : WorldContextList)
	{
		UWorld* World = WorldContext.World();
		if (!World || !World->IsGameWorld())
		{
			continue;
		}

		if (World->WorldComposition)
		{
			Result.Append(FString::Printf(TEXT("\n%s NetMode %d\n"), *World->GetName(), World->GetNetMode()));
			for (TObjectPtr<ULevelStreaming>& LevelStreaming : World->WorldComposition->TilesStreaming)
			{
				FString LevelName = FPackageName::GetShortName(LevelStreaming->PackageNameToLoad);
				Result.Append(FString::Printf(TEXT("\t %s --> Priority[%d], State[%s], BlockOnLoad[%d]"), 
										*LevelName, LevelStreaming->GetPriority(), ULevelStreaming::EnumToString(LevelStreaming->GetCurrentState()), LevelStreaming->bShouldBlockOnLoad));
			}
		}
		else
		{
			Result.Append(FString::Printf(TEXT("\n%s NetMode %d, not a world composition\n"), *World->GetName(), World->GetNetMode()));
		}
	}
	LogAndCopyToClipboard(Result);
}
#pragma endregion

#pragma region Character相关的Cheat
void UExCheatManagerExtension::Character(const FString& Param)
{
	int IntValue = 0;
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (UKismetSystemLibrary::ParseParam(Param, "ShowComponents"))
	{
		ShowActorComponets(Character);
	}
	else if (UExGameplayLibrary::ParseParamIntValue(Param, "-SyncMovement=", IntValue))
	{
		SetSyncCharacterMovement(Character, IntValue == 0 ? false : true);
	}
	else
	{
		ShowCharacterUsage();
	}
}

void UExCheatManagerExtension::ShowCharacterUsage()
{
	FString Usage;
	Usage.Append(FString::Printf(TEXT("Character -ShowComponents")));
	Usage.Append(FString::Printf(TEXT("\t\t-SyncMovement=0|1")));
	LogAndCopyToClipboard(Usage);
}
void UExCheatManagerExtension::SetSyncCharacterMovement(ACharacter* Character, bool Sync)
{
	UExCharacterMovementComponent* MovementComponent = Cast<UExCharacterMovementComponent>(Character->GetMovementComponent());
	if (!MovementComponent)
	{
		EXGAMEPLAY_LOG(Error, TEXT("Cannot Get UExCharacterMovementComponent"));
		return;
	}

	MovementComponent->SetMovementSyncEnable(Sync);
}
#pragma endregion

void UExCheatManagerExtension::ShowActorComponets(AActor* Actor)
{
	FString Result = FString::Printf(TEXT("Show Actor %s's Component. "), *GetNameSafe(Actor));
	if (Actor)
	{
		TSet<UActorComponent*> Components = Actor->GetComponents();
		Result += FString::Printf(TEXT("Num: %d\n"), Components.Num());

		for (UActorComponent* Component : Components)
		{
			Result.Append(FString::Printf(TEXT("%s\n"), *GetNameSafe(Component)));
		}
	}

	LogAndCopyToClipboard(Result);
}




