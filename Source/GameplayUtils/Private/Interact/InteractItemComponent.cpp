#include "Interact/InteractItemComponent.h"
#include "Interact/InteractSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"

//PRAGMA_DISABLE_OPTIMIZATION

UInteractItemComponent::UInteractItemComponent()
{
	//不需要tick
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractItemComponent::BeginPlay()
{
	Super::BeginPlay();

	InteractAssetMap.Empty();
	StateChangeHandlerMap.Empty();
	InteractHandlerMap.Empty();

	//优先从本身的配置中加载
	for (auto& StateHandlerItem : StateChangeHandlerConfig)
	{
		auto HandlerClass = StateHandlerItem.Value.LoadSynchronous();
		if (HandlerClass)
		{
			auto StateHandler = NewObject<UInteractItemHandler>(this, HandlerClass);
			if (StateHandler)
			{
				StateHandler->SetSourceObject(this);
				StateChangeHandlerMap.FindOrAdd(StateHandlerItem.Key) = StateHandler;
			}
		}
	}

	for (auto& HandlerConfigItem : InteractHandlerConfig)
	{
		auto HandlerClass = HandlerConfigItem.Value.LoadSynchronous();
		if (HandlerClass)
		{
			auto InteractHandler = NewObject<UInteractItemHandler>(this, HandlerClass);
			if (InteractHandler)
			{
				InteractHandler->SetSourceObject(this);
				InteractHandlerMap.FindOrAdd(HandlerConfigItem.Key) = InteractHandler;
			}
		}
	}

	//再从配置文件中加载
	for (auto& ConfigAssetPtr : InteractConfigAssets)
	{
		auto ConfigAsset = ConfigAssetPtr.LoadSynchronous();
		if (!ConfigAsset || ConfigAsset->ConfigData.ConfigName.IsNone())
		{
			continue;
		}

		//忽略重复配置
		FString ConfigName = ConfigAsset->ConfigData.ConfigName.ToString();
		if (InteractAssetMap.Find(ConfigName))
		{
			GAMEPLAYUTILS_LOG(Error, TEXT("%s, duplicate interact asset in %s: %s"), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), *ConfigName);
			continue;
		}
		InteractAssetMap.Add(ConfigName, ConfigAsset);

		//加载配置中的Handler
		if (ConfigAsset->StateChangeHandler)
		{
			auto& HandlerPtr = StateChangeHandlerMap.FindOrAdd(ConfigName);
			if (!HandlerPtr)
			{
				HandlerPtr = Cast<UInteractItemHandler>(UGameplayUtilsLibrary::CopyObject(ConfigAsset->StateChangeHandler, this));
				if (HandlerPtr)
				{
					HandlerPtr->SetSourceObject(this);
				}
			}
		}

		if (ConfigAsset->InteractHandler)
		{
			auto& HandlerPtr = InteractHandlerMap.FindOrAdd(ConfigName);
			if (!HandlerPtr)
			{
				HandlerPtr = Cast<UInteractItemHandler>(UGameplayUtilsLibrary::CopyObject(ConfigAsset->InteractHandler, this));
				if (HandlerPtr)
				{
					HandlerPtr->SetSourceObject(this);
				}
			}
		}
	}


	if (UInteractSubsystem* Subsystem = UInteractSubsystem::Get(this))
	{
		Subsystem->RegisterInteractItem(this, GetOwner());
	}
}

void UInteractItemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UInteractSubsystem* Subsystem = UInteractSubsystem::Get(this))
	{
		Subsystem->UnregisterInteractItem(this);
	}
}

void UInteractItemComponent::SetEnable(bool Enable, const FName& ConfigName)
{
	if (UInteractSubsystem* Subsystem = UInteractSubsystem::Get(this))
	{
		Subsystem->SetItemEnable(this, ConfigName, Enable);
	}
}

void UInteractItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UInteractItemComponent, InteractingInfoList, COND_None)
}

void UInteractItemComponent::OnInteractStateChange_Implementation(const FInteractInstanceData& InteractData)
{
	if (InteractStateChangeDelegate.IsBound())
	{
		InteractStateChangeDelegate.Broadcast(InteractData);
	}

	UInteractItemHandler* Handler = GetStateChangeHandler(InteractData.ConfigData.ConfigName);
	if (Handler)
	{
		Handler->NativeExecute(InteractData);
	}
}

bool UInteractItemComponent::CanInteract_Implementation(const FInteractInstanceData& InteractData)
{
	if (CanInteractDelegate.IsBound())
	{
		return CanInteractDelegate.Execute(InteractData);
	}

	return true;
}

void UInteractItemComponent::StartInteract_Implementation(const FInteractInstanceData& InteractData)
{
	if (StartInteractDelegate.IsBound())
	{
		StartInteractDelegate.Broadcast(InteractData);
	}

	if (UInteractItemHandler* Handler = GetInteractHandler(InteractData.ConfigData.ConfigName))
	{
		Handler->NativeExecute(InteractData);
	}
}

void UInteractItemComponent::EndInteract_Implementation(const FInteractInstanceData& InteractData)
{
	if (EndInteractDelegate.IsBound())
	{
		StartInteractDelegate.Broadcast(InteractData);
	}

	if (UInteractItemHandler* Handler = GetInteractHandler(InteractData.ConfigData.ConfigName))
	{
		Handler->NativeExecute(InteractData);
	}
}

int32 UInteractItemComponent::GetInteractingNum_Implementation(FName const& ConfigName)
{
	return InteractingInfoList.Num();
}

void UInteractItemComponent::AddInteractingPawn_Implementation(FName const& ConfigName, APawn* Pawn)
{
	if (GetOwner()->HasAuthority())
	{
		bool HasFound = false;
		for (auto& InteractingInfo : InteractingInfoList)
		{
			if (InteractingInfo.ConfigName == InteractingInfo.ConfigName && InteractingInfo.Instigator == Pawn)
			{
				HasFound = true;
			}
		}
		if (!HasFound)
		{
			auto* NewInteractingInfo = new (InteractingInfoList) FInteractReplicateData();
			NewInteractingInfo->ConfigName = ConfigName;
			NewInteractingInfo->Instigator = Pawn;
		}
	}
}

void UInteractItemComponent::RemoveInteractingPawn_Implementation(const FName& ConfigName, APawn* Pawn)
{
	if (GetOwner()->HasAuthority())
	{
		for (int i = InteractingInfoList.Num() - 1; i >= 0; i--)
		{
			FInteractReplicateData& InteractingInfo = InteractingInfoList[i];
			if (InteractingInfo.ConfigName == InteractingInfo.ConfigName && InteractingInfo.Instigator == Pawn)
			{
				InteractingInfoList.RemoveAt(i);
				return;
			}
		}
	}
}

TArray<FInteractConfigData> UInteractItemComponent::GetInteractConfigs_Implementation()
{
	TArray<FInteractConfigData> Result;
	for (auto& ConfigItem : InteractAssetMap)
	{
		if (ConfigItem.Value)
		{
			Result.Add(ConfigItem.Value->ConfigData);
		}
	}
	return MoveTemp(Result);
}

UInteractItemHandler* UInteractItemComponent::GetStateChangeHandler(const FName& ConfigName)
{
	if (auto HandlerPtr = StateChangeHandlerMap.Find(ConfigName.ToString()))
	{
		return *HandlerPtr;
	}
	return nullptr;
}

UInteractItemHandler* UInteractItemComponent::GetInteractHandler(const FName& ConfigName)
{
	if (auto HandlerPtr = InteractHandlerMap.Find(ConfigName.ToString()))
	{
		return *HandlerPtr;
	}
	return nullptr;
}

//PRAGMA_ENABLE_OPTIMIZATION