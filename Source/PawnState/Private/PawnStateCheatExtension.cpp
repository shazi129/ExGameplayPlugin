#include "PawnStateCheatExtension.h"
#include "PawnStateLibrary.h"
#include "PawnStateModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "ExMacros.h"
#include "PawnStateSubsystem.h"
#include "MessageCenter/MessageCenterSubsystem.h"

UPawnStateCheatExtension::UPawnStateCheatExtension()
{
	UMessageCenterSubsystem* MessageCenterSubsystem = UMessageCenterSubsystem::GetSubsystem(this);
	if (MessageCenterSubsystem != nullptr)
	{
		auto& DelegateInfo = MessageCenterSubsystem->GetMessageReceivedDelegateInfo(TAG_GetServerStates);
		DelegateInfo.Delegate.AddDynamic(this, &UPawnStateCheatExtension::OnMessageReceived);
	}
}

void UPawnStateCheatExtension::PawnState(const FString& Param)
{
	if (UKismetSystemLibrary::ParseParam(Param, "Show"))
	{
		ShowPawnState();
	}
	else if (UKismetSystemLibrary::ParseParam(Param, "ShowServer"))
	{
		ShowPawnStateServer();
	}
	else if (UKismetSystemLibrary::ParseParam(Param, "ShowASC"))
	{
		ShowASCTags();
	}
	else if (UKismetSystemLibrary::ParseParam(Param, "ShowASCServer"))
	{
		ShowASCTagsServer();
	}
	else if (UKismetSystemLibrary::ParseParam(Param, "ShowConfig"))
	{
		ShowConfig();
	}
	else
	{
		ShowPawnStateUsage();
	}
}

FString UPawnStateCheatExtension::GetPawnStateDebugString(UPawnStateComponent* Component)
{
	FString Result;
	if (Component)
	{
		const TArray<FPawnStateInstance>& PawnStateInstances = Component->GetPawnStateInstances();
		Result.Append(FString::Printf(TEXT("===============PawnState Count: %d===========\n"), PawnStateInstances.Num()));
		for (const FPawnStateInstance& Instance : PawnStateInstances)
		{
			Result.Append(FString::Printf(TEXT("%s\n"), *Instance.ToString()));
		}
	}
	else
	{
		Result.Append(FString::Printf(TEXT("Component is null\n")));
	}
	return Result;
}

FString UPawnStateCheatExtension::GetASCTagsDebugString(AActor* Actor)
{
	FString Result;
	if (Actor)
	{
		if (UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(Actor->GetComponentByClass(UAbilitySystemComponent::StaticClass())))
		{
			FGameplayTagContainer TagContainer;
			ASC->GetOwnedGameplayTags(TagContainer);

			Result.Append(FString::Printf(TEXT("===============Gameplay Tag Count: %d===========\n"), TagContainer.Num()));
			for (auto& Tag : TagContainer)
			{
				Result.Append(FString::Printf(TEXT("%s \t\t %d\n"), *Tag.ToString(), ASC->GetTagCount(Tag)));
			}
		}
		else
		{
			Result.Append(FString::Printf(TEXT("%s has no Ability system component\n"), *GetNameSafe(Actor)));
		}
	}
	else
	{
		Result.Append(FString::Printf(TEXT("cannot get GetPlayerPawn\n")));
	}
	return Result;
}

void UPawnStateCheatExtension::ShowPawnStateUsage()
{
	FString Usage;
	Usage.Append(FString::Printf(TEXT("PawnState -Show -ShowASC -ShowConfig\n")));
	Usage.Append(FString::Printf(TEXT("           -ShowServer -ShowASCServer\n")));
	LOG_AND_COPY(LogTemp, Log, Usage);
}

void UPawnStateCheatExtension::ShowPawnState()
{
	FString Result = UPawnStateCheatExtension::GetPawnStateDebugString(UPawnStateLibrary::GetLocalPawnStateComponent(this));
	LOG_AND_COPY(LogTemp, Log, Result);
}

void UPawnStateCheatExtension::ShowASCTags()
{
	FString Result = UPawnStateCheatExtension::GetASCTagsDebugString(UGameplayStatics::GetPlayerPawn(this, 0));
	LOG_AND_COPY(LogTemp, Log, Result);
}

void UPawnStateCheatExtension::ShowConfig()
{
	UPawnStateSubsystem* SubSystem = UPawnStateSubsystem::GetSubsystem(this);
	auto& RelationConfig = SubSystem->GetRelationConfig();

	TArray<FGameplayTag> StateList;
	RelationConfig.GetKeys(StateList);

	FString Result = FString::Printf(TEXT("=================State Num:%d===================\n"), StateList.Num());
	for (auto& State : StateList)
	{
		Result.Append(FString::Printf(TEXT("%s:"), *State.ToString()));
		auto& Relations = RelationConfig[State];
		for (auto& RelationItem : Relations)
		{
			Result.Append(FString::Printf(TEXT("  (%s, %d)"), *RelationItem.Key.ToString(), RelationItem.Value));
		}
		Result.Append("\n");
	}
	LOG_AND_COPY(LogTemp, Log, Result);
}

void UPawnStateCheatExtension::ShowPawnStateServer()
{
	UMessageCenterSubsystem* MessageCenterSubsystem = UMessageCenterSubsystem::GetSubsystem(this);
	FGameplayMessage Message(TAG_GetServerStates);
	MessageCenterSubsystem->SendMsgToServer(Message);
}

void UPawnStateCheatExtension::ShowASCTagsServer()
{
	UMessageCenterSubsystem* MessageCenterSubsystem = UMessageCenterSubsystem::GetSubsystem(this);
	FGameplayMessage Message(TAG_GetServerTags);
	MessageCenterSubsystem->SendMsgToServer(Message);
}

void UPawnStateCheatExtension::OnMessageReceived(const FGameplayMessage& Message)
{
	if (Message.MsgTag == TAG_GetServerStates)
	{

	}
	else if (Message.MsgTag == TAG_GetServerStates)
	{

	}
}
