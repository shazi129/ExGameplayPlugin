#include "PawnStateCheatExtension.h"
#include "PawnStateLibrary.h"
#include "PawnStateModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ExMacros.h"

void UPawnStateCheatExtension::PawnState(const FString& Param)
{
	if (UKismetSystemLibrary::ParseParam(Param, "Show"))
	{
		ShowPawnState();
	}
	else if (UKismetSystemLibrary::ParseParam(Param, "ShowASC"))
	{
		ShowASCTags();
	}
	else
	{
		ShowPawnStateUsage();
	}
}

void UPawnStateCheatExtension::ShowPawnStateUsage()
{
	FString Usage;
	Usage.Append(FString::Printf(TEXT("Scene -Show -ShowASC\n")));
	LOG_AND_COPY(LogTemp, Log, Usage);
}

void UPawnStateCheatExtension::ShowPawnState()
{
	FString Result;
	if (UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetLocalPawnStateComponent(this))
	{
		const TArray<FPawnStateInstance>& PawnStateInstances = PawnStateComponent->GetPawnStateInstances();
		for (const FPawnStateInstance& Instance : PawnStateInstances)
		{
			Result.Append(FString::Printf(TEXT("%s\n"), *Instance.ToString()));
		}
	}
	else
	{
		Result.Append(FString::Printf(TEXT("cannot get local pawn state component\n")));
	}
	LOG_AND_COPY(LogTemp, Log, Result);
}

void UPawnStateCheatExtension::ShowASCTags()
{
	FString Result;
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(Pawn->GetComponentByClass(UAbilitySystemComponent::StaticClass())))
		{
			FGameplayTagContainer TagContainer;
			ASC->GetOwnedGameplayTags(TagContainer);
			for (auto& Tag : TagContainer)
			{
				Result.Append(FString::Printf(TEXT("%s \t\t %d\n"), *Tag.ToString(), ASC->GetTagCount(Tag)));
			}
		}
		else
		{
			Result.Append(FString::Printf(TEXT("%s has no Ability system component\n"), *GetNameSafe(Pawn)));
		}
	}
	else
	{
		Result.Append(FString::Printf(TEXT("cannot get GetPlayerPawn\n")));
	}
	LOG_AND_COPY(LogTemp, Log, Result);
}