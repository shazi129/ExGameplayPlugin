#include "FunctionLibraries/GameplayUtilsLibrary.h"
#include "GameplayUtilsModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerState.h"
#include "FunctionLibraries/PathHelperLibrary.h"
#include "GameplayUtilsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"

FARFilter FGameplayARFilter::MakeARFilter() const
{
	FARFilter ArFilter;

	for (auto Class : Classes)
	{
		ArFilter.ClassPaths.Add(Class->GetClassPathName());
	}
	ArFilter.PackagePaths = PackagePaths;
	ArFilter.bRecursivePaths = bRecursivePaths;
	ArFilter.bIncludeOnlyOnDiskAssets = bIncludeOnlyOnDiskAssets;
	return MoveTemp(ArFilter);
}

FGameplayAssetData::FGameplayAssetData(const FAssetData& AssetData)
{
	PackageName = AssetData.PackageName;
	PackagePath = AssetData.PackagePath;
	AssetName = AssetData.AssetName;
}

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


void UGameplayUtilsLibrary::FilterActors(const TArray<AActor*>& Actors, const FFilterActorCondition FilterCondition, TArray<AActor*>& OutActors)
{
	check(&Actors != &OutActors)

	for (auto& Actor : Actors)
	{
		if (FilterCondition.FilterActor(Actor))
		{
			OutActors.Add(Actor);
		}
	}
}

bool UGameplayUtilsLibrary::CheckExecNetMode(const UObject* WorldContextObject, EExecNetMode ExecNetMode)
{
	if (ExecNetMode == EExecNetMode::E_Aways)
	{
		return true;
	}

	if (!WorldContextObject)
	{
		return false;
	}

	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		ENetMode NetMode = World->GetNetMode();

		switch(ExecNetMode)
		{
			case EExecNetMode::E_Server:
			{
				return NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer  || NetMode == ENetMode::NM_Standalone;
			}
			case EExecNetMode::E_Client:
			{
				return NetMode == ENetMode::NM_Client || NetMode == ENetMode::NM_ListenServer || NetMode == ENetMode::NM_Standalone;
			}
			case EExecNetMode::E_ExactClient:
			{
				return NetMode == ENetMode::NM_Client;
			}
			case EExecNetMode::E_ExactServer:
			{
				return NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer;
			}
			case EExecNetMode::E_Client_Server:
			{
				return NetMode == ENetMode::NM_Standalone || NetMode == ENetMode::NM_ListenServer;
			}
			case EExecNetMode::E_Standalone:
			{
				return NetMode == ENetMode::NM_Standalone;
			}
			default:
				return false;
		}
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
	if (!Actor)
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
		if (ComponentName.IsEmpty() || Component->GetName().Equals(ComponentName))
		{
			return Component;
		}
	}
	return nullptr;
}

TArray<UActorComponent*> UGameplayUtilsLibrary::GetComponentsByClass(AActor* Actor, UClass* CompClass)
{
	TArray<UActorComponent*> Result;
	if (!Actor)
	{
		return MoveTemp(Result);
	}

	if (CompClass == nullptr)
	{
		CompClass = UActorComponent::StaticClass();
	}
	Actor->GetComponents(CompClass, Result);
	return MoveTemp(Result);
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

bool UGameplayUtilsLibrary::TickMoveTo(float DeltaTime, USceneComponent* TargetComp, const FVector& TargetLocation, float Speed)
{
	if (!TargetComp)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, target component is null"), *FString(__FUNCTION__));
		return false;
	}

	FVector CurrentLocation = TargetComp->GetComponentLocation();
	FVector Direction = TargetLocation - CurrentLocation;
	if (Direction.IsNearlyZero(0.01))
	{
		return true;
	}

	//如果下一帧会走过，直接设置到目标位置
	if (Direction.Size() < Speed * DeltaTime)
	{
		TargetComp->SetWorldLocation(TargetLocation, false, nullptr, ETeleportType::None);
		return true;
	}

	//根据速度算出移动距离
	Direction.Normalize(0.01);
	FVector NewLocation = CurrentLocation + Direction * Speed * DeltaTime;
	TargetComp->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::None);

	return false;
}

FBoxSphereBounds UGameplayUtilsLibrary::GetLocalBounds(USceneComponent* SceneComponent)
{
	if (SceneComponent)
	{
		return SceneComponent->GetLocalBounds();
	}

	FBoxSphereBounds NewBounds;
	NewBounds.Origin = FVector::ZeroVector;
	NewBounds.BoxExtent = FVector::ZeroVector;
	NewBounds.SphereRadius = 0.f;
	return MoveTemp(NewBounds);
}

TArray<FGameplayTag> UGameplayUtilsLibrary::MakeTagArrayWithContainer(const FGameplayTagContainer& Container)
{
	TArray<FGameplayTag> Result;
	for (const FGameplayTag& Tag : Container)
	{
		Result.Add(Tag);
	}

	return MoveTemp(Result);
}

TArray<FGameplayAssetData> UGameplayUtilsLibrary::GetAssets(const FGameplayARFilter& GameplayARFilter)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;
	AssetDataList.Empty();

	FARFilter ARFilter = GameplayARFilter.MakeARFilter();
	AssetRegistry.GetAssets(ARFilter, AssetDataList);

	TArray<FGameplayAssetData> Result;
	for (auto& AssetData : AssetDataList)
	{
		Result.Add(FGameplayAssetData(AssetData));
	}
	return MoveTemp(Result);
}

void UGameplayUtilsLibrary::SetConnectionTimeout(UObject* ContextObject, float Seconds)
{
	if (!ContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UGameplayUtilsLibrary::SetConnectionTimeout error, context object is null"));
		return;
	}
	UWorld* World = ContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UGameplayUtilsLibrary::SetConnectionTimeout failed, cannot find world by %s"), *GetNameSafe(ContextObject));
		return;
	}

	UNetDriver* NetDriver = World->GetNetDriver();
	if (!NetDriver)
	{
		UE_LOG(LogTemp, Error, TEXT("UGameplayUtilsLibrary::SetConnectionTimeout failed, cannot find NetDriver by %s"), *GetNameSafe(World));
		return;
	}

	NetDriver->bNoTimeouts = false;
	NetDriver->ConnectionTimeout = Seconds;
	NetDriver->TimeoutMultiplierForUnoptimizedBuilds = 1;
}

bool UGameplayUtilsLibrary::CompareDigits(float A, float B, EDataCompareMode CompareMode)
{
	switch (CompareMode)
	{
	case EDataCompareMode::SmallerThan:
		return A < B;
	case EDataCompareMode::BiggerThan:
		return A > B;
	case EDataCompareMode::Equal:
		return A == B;
	default:
		break;
	}
	return false;
}

void UGameplayUtilsLibrary::ShutdownWorldNetDriver(UWorld* World)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UPartyGameBPLibrary::ShutdownWorldNetDriver failed, world is null"));
	}

	UNetDriver* NetDriver = World->GetNetDriver();
	if (!NetDriver)
	{
		return;
	}

	//如果不在tick中，可以当帧关掉，否则等到下一帧
	if (!NetDriver->IsInTick())
	{
		GEngine->ShutdownWorldNetDriver(World);
		return;
	}

	World->GetTimerManager().SetTimerForNextTick([World]()
	{
		GEngine->ShutdownWorldNetDriver(World);
	});
}

void UGameplayUtilsLibrary::CleanUpNetConnection(APlayerController* PlayerController)
{
	UE_LOG(LogTemp, Log, TEXT("UGameplayUtilsLibrary::CleanUpNetConnection %s"), *GetNameSafe(PlayerController));
	if (!PlayerController || !PlayerController->NetConnection)
	{
		return;
	}

	PlayerController->NetConnection->Close();
	//PlayerController->NetConnection->CleanUp();
}

UObject* UGameplayUtilsLibrary::TryLoadSoftObjectPath(const FSoftObjectPath& SoftObjectPath, UClass* ObjectClass)
{
	UObject* Object = SoftObjectPath.TryLoad();
	if (Object && ObjectClass && Object->IsA(ObjectClass))
	{
		return Object;
	}
	return nullptr;
}