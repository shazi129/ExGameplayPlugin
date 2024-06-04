#include "FunctionLibraries/GameplayUtilsLibrary.h"
#include "GameplayUtilsModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerState.h"
#include "FunctionLibraries/PathHelperLibrary.h"
#include "GameplayUtilsModule.h"

bool UGameplayUtilsLibrary::ExecCommand(const FString& Command)
{
	if (!GEngine)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, GEngine is Null"), *FString(__FUNCTION__));
		return false;
	}

	GAMEPLAYUTILS_LOG(Log, TEXT("%s[%s]"), *FString(__FUNCTION__), *Command);

	UWorld* World = nullptr;

	ULocalPlayer* Player = GEngine->GetDebugLocalPlayer();
	if (Player)
	{
		World = Player->GetWorld();
		if (World)
		{
			if (Player->Exec(World, *Command, *GLog))
			{
				return true;
			}
		}
		else
		{
			GAMEPLAYUTILS_LOG(Error, TEXT("%s error, PlayerWorld is Null"), *FString(__FUNCTION__));
			return false;
		}
	}

	if (!World)
	{
		World = GEngine->GetWorld();
	}
	return GEngine->Exec(World, *Command);
}

bool UGameplayUtilsLibrary::FilterActorClasses(AActor* Actor, const TArray<TSubclassOf<AActor>>& ActorClasses)
{
	if (!Actor) return false;
	if (ActorClasses.IsEmpty())
	{
		return true;
	}
	for (auto& ActorClass : ActorClasses)
	{
		if (Actor->IsA(ActorClass))
		{
			return true;
		}
	}
	return false;
}

void UGameplayUtilsLibrary::FilterActors(const TArray<AActor*>& Actors, const FFilterActorCondition FilterCondition, TArray<AActor*>& OutActors)
{
	check(&Actors != &OutActors)

	for (auto& Actor : Actors)
	{
		if (!Actor)
		{
			continue;
		}

		//检查是否ignore
		if (!FilterCondition.FilterIgnoreActors(Actor))
		{
			continue;
		}

		if (!FilterCondition.FilterExcludeComponentClasses(Actor))
		{
			continue;
		}

		if (!FilterCondition.FilterRequireComponentClasses(Actor))
		{
			continue;
		}

		if (!UGameplayUtilsLibrary::FilterActorClasses(Actor, FilterCondition.ActorClasses))
		{
			continue;
		}

		OutActors.Add(Actor);
	}
}

bool UGameplayUtilsLibrary::CheckExecNetMode(const UObject* WorldContextObject, EExecNetMode ExecNetMode)
{
	if (!WorldContextObject)
	{
		return false;
	}
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		ENetMode NetMode = World->GetNetMode();
		if (ExecNetMode == EExecNetMode::E_Server)
		{
			return NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer  || NetMode == ENetMode::NM_Standalone;
		}
		else if (ExecNetMode == EExecNetMode::E_Client)
		{
			return NetMode == ENetMode::NM_Client || NetMode == ENetMode::NM_ListenServer || NetMode == ENetMode::NM_Standalone;
		}
		else if (ExecNetMode == EExecNetMode::E_ExactClient)
		{
			return NetMode == ENetMode::NM_Client;
		}
		else if (ExecNetMode == EExecNetMode::E_ExactServer)
		{
			return NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer;
		}
		return true;
	}
	return false;
}

bool UGameplayUtilsLibrary::IsLocalControled(AActor* Actor)
{
	if (!Actor) return false;

	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		return false;
	}

	const ENetMode NetMode = World->GetNetMode();
	if (NetMode == NM_Standalone)
	{
		return true;
	}

	ENetRole LocalRole = Actor->GetLocalRole();
	if (NetMode == NM_Client && LocalRole == ROLE_AutonomousProxy)
	{
		return true;
	}

	ENetRole RemoteRole = Actor->GetRemoteRole();
	if (RemoteRole != ROLE_AutonomousProxy && LocalRole == ROLE_Authority)
	{
		return true;
	}

	return false;
}

UObject* UGameplayUtilsLibrary::GetDefaultObject(UClass* ObjectClass)
{
	if (ObjectClass == nullptr)
	{
		return nullptr;
	}
	return ObjectClass->GetDefaultObject();
}

APawn* UGameplayUtilsLibrary::GetPawnByPlayerState(APlayerState* PlayerState)
{
	if (PlayerState)
	{
		if (AController* Controller = PlayerState->GetOwner<AController>())
		{
			return Controller->GetPawn();
		}
	}
	return nullptr;
}

APlayerState* UGameplayUtilsLibrary::GetPlayerStateByActor(AActor* Actor)
{
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->GetPlayerState();
	}
	return nullptr;
}

UActorComponent* UGameplayUtilsLibrary::GetComponentByTag(AActor* Actor, const FName& Tag)
{
	if (Actor)
	{
		TArray<UActorComponent*> Components = Actor->GetComponentsByTag(UActorComponent::StaticClass(), Tag);
		if (Components.Num() > 0)
		{
			return Components[0];
		}
	}
	return nullptr;
}

UActorComponent* UGameplayUtilsLibrary::GetComponentByClass(AActor* Actor, TSubclassOf<UActorComponent> ComponentClass)
{
	if (Actor && ComponentClass)
	{
		return Actor->GetComponentByClass(ComponentClass);
	}
	return nullptr;
}

FString UGameplayUtilsLibrary::GetNameSafe(const UObject* Object)
{
	return ::GetNameSafe(Object);
}

FString UGameplayUtilsLibrary::GetContextWorldName(const UObject* Object)
{
	if (!Object)
	{
		return "None";
	}

	UWorld* ContextWorld = Object->GetWorld();
	if (!ContextWorld)
	{
		return "None";
	}

	if (UPackage* Package = ContextWorld->GetPackage())
	{
		FString PackageShortName = FPackageName::GetShortName(Package->GetName());
		PackageShortName.RemoveFromStart(ContextWorld->StreamingLevelsPrefix);
		return PackageShortName;
	}
	return "None";
}

UObject* UGameplayUtilsLibrary::CopyObject(UObject* TemplateObject, UObject* Outer, FName Name)
{
	if (TemplateObject)
	{
		FStaticConstructObjectParameters Params(TemplateObject->GetClass());
		Params.Outer = Outer;
		Params.Name = Name;
		Params.Template = TemplateObject;
		Params.bCopyTransientsFromClassDefaults = true;
		return StaticConstructObject_Internal(Params);
	}
	return nullptr;
}

TArray<UActorComponent*> UGameplayUtilsLibrary::GetComponentsByTickEnable(AActor* Actor, bool Enabled)
{
	TArray<UActorComponent*> Components;
	if (!Actor)
	{
		return MoveTemp(Components);
	}

	const TSet<UActorComponent*>&  ActorComponents = Actor->GetComponents();
	for (UActorComponent* ActorComponent : ActorComponents)
	{
		if (ActorComponent->IsComponentTickEnabled() == Enabled)
		{
			Components.Add(ActorComponent);
		}
	}

	return MoveTemp(Components);
}

void UGameplayUtilsLibrary::SetComponentsTickEnable(TArray<UActorComponent*>& Components, bool Enable)
{
	for (UActorComponent* Component : Components)
	{
		if (Component)
		{
			Component->SetComponentTickEnabled(Enable);
		}
	}
}

bool UGameplayUtilsLibrary::IsSameWorld(const UWorld* World, const TSoftObjectPtr<UWorld>& TargetWorld)
{
	if (World && !TargetWorld.IsNull())
	{
		FString TargetWorldPath = TargetWorld.GetLongPackageName();
		FString CurrentWorldPath = UPathHelperLibrary::GetPackageFullName(World);
		return TargetWorldPath == CurrentWorldPath;
	}
	return false;
}

bool UGameplayUtilsLibrary::IsCurrentWorld(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld>& TargetWorld)
{
	if (!WorldContextObject || TargetWorld.IsNull())
	{
		return false;
	}

	if (UWorld* World = WorldContextObject->GetWorld())
	{
		return UGameplayUtilsLibrary::IsSameWorld(World, TargetWorld);
	}

	return false;
}

bool UGameplayUtilsLibrary::IsGameWorld(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return false;
	}

	if (World->GetName().Equals(FString("Untitled")))
	{
		return false;
	}

	return true;
}

FTransform UGameplayUtilsLibrary::GetBoneTransform(USkinnedMeshComponent* SkinnedMeshComponent, int32 BoneIndex)
{
	if (SkinnedMeshComponent)
	{
		return SkinnedMeshComponent->GetBoneTransform(BoneIndex);
	}
	return FTransform::Identity;
}

TMap<FName, FTransform> UGameplayUtilsLibrary::GetBonesTransform(USkeletalMeshComponent* SkeletalMeshComponent, const TArray<FName>& BoneNames)
{
	TMap<FName, FTransform> Result;

	if (!SkeletalMeshComponent || BoneNames.IsEmpty())
	{
		return MoveTemp(Result);
	}
	
	USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(SkeletalMeshComponent->GetSkinnedAsset());
	if (!SkeletalMesh)
	{
		return MoveTemp(Result);
	}

	for (auto& BoneName : BoneNames)
	{
		if (!Result.Contains(BoneName))
		{
			int32 BoneIndex = SkeletalMeshComponent->GetBoneIndex(BoneName);

			FTransform Transform = SkeletalMeshComponent->GetBoneTransform(BoneIndex, SkeletalMeshComponent->GetComponentTransform());
			Result.Add(BoneName, Transform);
		}
	}

	return MoveTemp(Result);
}

UActorComponent* UGameplayUtilsLibrary::GetComponentByName(AActor* Actor, const FString& ComponentName, UClass* CompClass)
{
	if (!Actor || ComponentName.IsEmpty())
	{
		return nullptr;
	}

	if (CompClass == nullptr)
	{
		CompClass = UActorComponent::StaticClass();
	}

	TArray<UActorComponent*> Components;
	Actor->GetComponents(CompClass, Components);
	if (Components.Num() == 0)
	{
		return nullptr;
	}

	for (auto Component : Components)
	{
		if (Component->GetName().Equals(ComponentName))
		{
			return Component;
		}
	}
	return nullptr;
}

TArray<int32> UGameplayUtilsLibrary::RandomItemsByWeight(const UObject* WorldContextObject, const TArray<FItemWeightsEntry>& ItemWeightsEntryList, int32 ItemCount)
{
	TArray<int32> Result;

	do 
	{
		if (!WorldContextObject || ItemCount <= 0 || ItemWeightsEntryList.Num() == 0)
		{
			break;
		}

		UWorld* World = WorldContextObject->GetWorld();
		if (!World)
		{
			break;
		}

		//找到对应的World，先找TargetWorld匹配的，找不到用TargetWorld为null的
		const FItemWeightsEntry* ItemWeightsEntryPtr = nullptr;

		for (auto& WeightsEntry : ItemWeightsEntryList)
		{
			if (UGameplayUtilsLibrary::IsSameWorld(World, WeightsEntry.TargetWorld))
			{
				ItemWeightsEntryPtr = &WeightsEntry;
				break;
			}
			else if (WeightsEntry.TargetWorld.IsNull())
			{
				ItemWeightsEntryPtr = &WeightsEntry;
			}
		}

		if (!ItemWeightsEntryPtr)
		{
			break;
		}

		int32 TotalWeight = 0;
		for (auto& ItemWeight : ItemWeightsEntryPtr->ItemWeightList)
		{
			TotalWeight += ItemWeight.Weight;
		}

		for (int ItemIndex = 0; ItemIndex < ItemCount; ItemIndex++)
		{
			int32 RandomWeight = FMath::RandRange(0, TotalWeight - 1);

			// 根据随机数选择一个数字
			for (auto& ItemWeight : ItemWeightsEntryPtr->ItemWeightList)
			{
				if (RandomWeight < ItemWeight.Weight)
				{
					Result.Add(ItemWeight.ItemID);
					break;
				}
				RandomWeight -= ItemWeight.Weight;
			}
		}
	}
	while(false);

	return MoveTemp(Result);
}

void UGameplayUtilsLibrary::SetInstanceNotifyRBCollision(UPrimitiveComponent* Component, bool bNewNotifyCollision)
{
	if (Component)
	{
		Component->BodyInstance.SetInstanceNotifyRBCollision(bNewNotifyCollision);
	}
}

bool UGameplayUtilsLibrary::IsValid(UObject* Object)
{
	if (!Object || !IsValidChecked(Object))
	{
		return false;
	}

	if (AActor* Actor = Cast<AActor>(Object))
	{
		if (Actor->IsActorBeingDestroyed())
		{
			return false;
		}
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(Object))
	{
		AActor* Owner = Component->GetOwner();
		if (!Owner || Owner->IsActorBeingDestroyed())
		{
			return false;
		}
	}
	return true;
}

