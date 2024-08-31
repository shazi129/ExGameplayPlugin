// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractSubsystem.h"
#include "Subsystems/GameplayAsyncTaskSubsystem.h"
#include "Interact/InteractManagerComponent.h"
#include "Macros/SubsystemMacros.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"

UInteractSubsystem* UInteractSubsystem::Get(const UObject* WorldContextObject)
{
	GET_WORLD_SUBSYSTEM(LogGameplayUtils, UInteractSubsystem, WorldContextObject);
}

bool UInteractSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!UGameplayUtilsLibrary::IsGameWorld(Outer))
	{
		return false;
	}
	return Super::ShouldCreateSubsystem(Outer);
}

void UInteractSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bEnabled = true;
	bDataDirty = true;

	UGameplayAsyncTaskSubsystem* AsyncTaskSubsystem = Collection.InitializeDependency<UGameplayAsyncTaskSubsystem>();

	ENetMode NetMode = GetWorld()->GetNetMode();
	bInServer = NetMode == NM_DedicatedServer;

	//只有客户端需要Tick
	if (!bInServer && AsyncTaskSubsystem)
	{
		TickName = AsyncTaskSubsystem->CreateAndRegisterTickFunction(this, "InteractTick", ETickingGroup::TG_PrePhysics);
		if (auto TickFunctionPtr = AsyncTaskSubsystem->GetTickFunction(TickName))
		{
			TickFunctionPtr->TickDelegate.BindDynamic(this, &UInteractSubsystem::Tick);
		}
	}
}


void UInteractSubsystem::Deinitialize()
{
	if (!TickName.IsNone())
	{
		if (UGameplayAsyncTaskSubsystem* AsyncTaskSubsystem = UGameplayAsyncTaskSubsystem::Get(this))
		{
			AsyncTaskSubsystem->RemoveTickFunction(TickName);
		}
	}

	Super::Deinitialize();
}

void UInteractSubsystem::Tick(float Seconds)
{
	if (bEnabled && !bInServer)
	{
		RebuildInteractData();
	}
}

void UInteractSubsystem::RegisterInteractItem(TScriptInterface<IInteractItemInterface> InteractItem, AActor* ItemActor)
{
	AddInteracts(InteractItem, ItemActor);
}


bool UInteractSubsystem::AddInteracts(TScriptInterface<IInteractItemInterface> InteractItem, AActor* ItemActor)
{
	UObject* ItemObject = InteractItem.GetObject();
	if (!ItemActor || !ItemObject)
	{
		return false;
	}

	FObjectInstanceDataInfo* InstanceDataInfoPtr = InteractInstanceMap.Find(ItemObject);
	if (InstanceDataInfoPtr)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s add duplicate config for %s"), *FString(__FUNCTION__), *GetNameSafe(ItemObject));
		return false;
	}

	FObjectInstanceDataInfo& InstanceDataInfo = InteractInstanceMap.Add(ItemObject);

	//获取配置
	TArray<FInteractConfigData> ConfigDataList = IInteractItemInterface::Execute_GetInteractConfigs(ItemObject);
	for (auto ConfigData : ConfigDataList)
	{
		//添加Instance数据
		FInteractInstanceData * InstanceData = new(InstanceDataInfo.InstanceDataList) FInteractInstanceData(InteractItem, ItemActor, ConfigData);

		//修正数据
		for (int i = InstanceData->ConfigData.Ranges.Num() - 1; i >= 0; i--)
		{
			if (InstanceData->ConfigData.Ranges[i].Angle <= 0)
			{
				InstanceData->ConfigData.Ranges.RemoveAt(i);
			}
		}
	}
	
	return true;
}


bool UInteractSubsystem::CanInteract(const TScriptInterface<IInteractItemInterface>& InteractItem, APawn* Instigator, const FName& ConfigName)
{
	UObject* Object = InteractItem.GetObject();
	if (!UGameplayUtilsLibrary::IsValid(Object))
	{
		return false;
	}

	auto InstanceData = FindInstance(Object, ConfigName);
	if (!InstanceData)
	{
		return false;
	}

	//TODO: 没有Tick的时候，这里可再校验一遍距离
	if (TickName.IsNone())
	{

	}

	return IInteractItemInterface::Execute_CanInteract(Object, *InstanceData);
}

void UInteractSubsystem::UnregisterInteractItem(TScriptInterface<IInteractItemInterface> InInteractItem)
{
	UObject* ItemObject = InInteractItem.GetObject();
	if (!ItemObject)
	{
		return;
	}

	//重置下状态，通知出去做收尾处理
	if (!bInServer)
	{
		FObjectInstanceDataInfo* ObjectInstanceDataInfoPtr = InteractInstanceMap.Find(InInteractItem.GetObject());
		if (ObjectInstanceDataInfoPtr)
		{
			for (int i = 0; i < ObjectInstanceDataInfoPtr->InstanceDataList.Num(); i++)
			{
				FInteractInstanceData& InstanceData = ObjectInstanceDataInfoPtr->InstanceDataList[i];
				if (InstanceData.InteractState != EInteractState::E_None)
				{
					InstanceData.InteractState = EInteractState::E_None;
					NotifyStatChange(ItemObject, InstanceData);
				}
			}
		}
	}

	InvalidateObjects.Add(ItemObject);

	//没有tick的话，手动rebuild一次
	if (TickName.IsNone())
	{
		RebuildInteractData();
	}
}

void UInteractSubsystem::SetInteractPawn(APawn* Pawn)
{
	InteractPawn = Pawn;
	PawnInteractRange.Reset();

	if (InteractPawn.IsValid())
	{
		UActorComponent* ManagerComponent = InteractPawn->GetComponentByClass(UInteractManagerComponent::StaticClass());
		if (UInteractManagerComponent * InteractManagerComponent = Cast<UInteractManagerComponent>(ManagerComponent))
		{
			PawnInteractRange = InteractManagerComponent->InteractRange;
		}
	}
}

FInteractInstanceData* UInteractSubsystem::FindInstance(UObject* ItemObject, const FName& ConfigName)
{
	auto InstanceDataListPtr = InteractInstanceMap.Find(ItemObject);
	if (!InstanceDataListPtr)
	{
		return nullptr;
	}

	for (int i = 0; i < InstanceDataListPtr->InstanceDataList.Num(); i++)
	{
		FInteractInstanceData& InstanceData = InstanceDataListPtr->InstanceDataList[i];
		if (ConfigName == InstanceData.ConfigData.ConfigName)
		{
			return &InstanceData;
		}
	}
	return nullptr;
}

void UInteractSubsystem::SetEnable(bool Enable)
{
	bEnabled = Enable;
	//强制更新一遍
	RebuildInteractData(true);
}

bool UInteractSubsystem::GetEnable()
{
	return bEnabled;
}

void UInteractSubsystem::UpdateItem(FInteractInstanceData& InteractInstanceData)
{
	//异常情况不做处理
	if (!InteractPawn.IsValid() || !InteractInstanceData.IsValid())
	{
		return;
	}

	//如果不可用，状态直接设为None
	if (!bEnabled || !InteractInstanceData.Enable)
	{
		InteractInstanceData.NeedNofity = InteractInstanceData.InteractState != EInteractState::E_None;
		InteractInstanceData.InteractState = EInteractState::E_None;
		return;
	}

	//达到了交互最大值，所有非交互状态的item都置None
	if (InteractInstanceData.ConfigData.MaxInteractNum > 0)
	{
		int32 CurrentNum = IInteractItemInterface::Execute_GetInteractingNum(InteractInstanceData.ItemInterface.GetObject(), InteractInstanceData.ConfigData.ConfigName);
		if (CurrentNum >= InteractInstanceData.ConfigData.MaxInteractNum && InteractInstanceData.InteractState != EInteractState::E_Interacting)
		{
			InteractInstanceData.NeedNofity = InteractInstanceData.InteractState != EInteractState::E_None;
			InteractInstanceData.InteractState = EInteractState::E_None;
			return;
		}
	}

	InteractInstanceData.Instigator = InteractPawn.Get();
	FTransform PawnTransform = InteractPawn->GetTransform();
	FTransform ItemTransform = InteractInstanceData.ItemActor->GetTransform();

	//玩家交互方向
	FVector PawnForward = InteractPawn->GetActorForwardVector();
	if (!PawnInteractRange.Direction.IsNearlyZero())
	{
		PawnForward = UKismetMathLibrary::TransformDirection(PawnTransform, PawnInteractRange.Direction);
	}

	//物品相对于玩家的方向, 世界坐标
	FVector ItemLocation = ItemTransform.GetLocation() + InteractInstanceData.ConfigData.CenterOffset;
	FVector ItemDirection = (ItemLocation - InteractPawn->GetActorLocation());

	//物品相对玩家的距离
	InteractInstanceData.Distance = ItemDirection.Size();

	//物品与玩家相对玩家交互方向的夹角cos值
	ItemDirection.Normalize();
	InteractInstanceData.Angle = FVector::DotProduct(PawnForward, ItemDirection);


	//1. 先按距离做判断
	EInteractState InteractState = EInteractState::E_None;
	int32 SubPotentialIndex = -1;

	//在交互半径
	if (InteractInstanceData.Distance <= InteractInstanceData.ConfigData.InteractiveRadius)
	{
		InteractState = EInteractState::E_Interactive;

		//是否在manager的可交互角度范围内
		if (PawnInteractRange.Angle > 0)
		{
			//物体配置朝向和玩家对于物体的方向
			FVector ItemToPawnLocalDirection = UKismetMathLibrary::InverseTransformDirection(PawnTransform, ItemDirection);
			FVector PawnForwardLocalDirction = UKismetMathLibrary::InverseTransformDirection(PawnTransform, PawnForward);

			IgnoreVectorAxis(ItemToPawnLocalDirection, PawnInteractRange.IgnoreAxis);
			IgnoreVectorAxis(PawnForwardLocalDirction, PawnInteractRange.IgnoreAxis);

			float ItemCosAngle = FVector::DotProduct(ItemToPawnLocalDirection, PawnForwardLocalDirction);
			if (ItemCosAngle < FMath::Cos(PawnInteractRange.Angle * PI / 180))
			{
				InteractState = EInteractState::E_Potential;
			}
		}

		//是否在Item的可交互范围内
		if (InteractInstanceData.ConfigData.Ranges.Num() > 0)
		{
			bool HasFound = InteractInstanceData.ConfigData.Ranges.Num() == 0;
			for (int i = 0; i < InteractInstanceData.ConfigData.Ranges.Num(); i++)
			{
				const FInteractRange& ItemRange = InteractInstanceData.ConfigData.Ranges[i];
				if (ItemRange.Angle >= 0 && !ItemRange.Direction.IsNearlyZero())
				{
					//物体配置朝向和玩家对于物体的方向
					FVector ItemLocalDirection = ItemRange.Direction;
					FVector PawnToItemLocalDirction = UKismetMathLibrary::InverseTransformDirection(ItemTransform, -ItemDirection);

					IgnoreVectorAxis(ItemLocalDirection, ItemRange.IgnoreAxis);
					IgnoreVectorAxis(PawnToItemLocalDirction, ItemRange.IgnoreAxis);

					float ItemCosAngle = FVector::DotProduct(ItemLocalDirection, PawnToItemLocalDirction);

					if (ItemCosAngle > FMath::Cos(ItemRange.Angle * PI / 180))
					{
						HasFound = true;
						break;
					}
				}
			}
			if (!HasFound)
			{
				InteractState = EInteractState::E_Potential;
			}
		}
	}

	//在提示半径
	if (InteractState != EInteractState::E_Interactive && InteractInstanceData.ConfigData.PotentialRadiusList.Num() > 0)
	{
		if (InteractInstanceData.Distance <= InteractInstanceData.ConfigData.PotentialRadiusList[0])
		{
			InteractState = EInteractState::E_Potential;
			for (int i = 0; i < InteractInstanceData.ConfigData.PotentialRadiusList.Num(); i++)
			{
				if (InteractInstanceData.Distance < InteractInstanceData.ConfigData.PotentialRadiusList[i])
				{
					SubPotentialIndex = i;
				}
				else
				{
					break;
				}
			}
		}
	}

	//如果正在交互时，离开了交互范围
	if (InteractInstanceData.InteractState == EInteractState::E_Interacting)
	{
		if (InteractState == EInteractState::E_Interactive)
		{
			InteractState = EInteractState::E_Interacting;
		}
		else
		{
			InteractInstanceData.NeedNofity = true;
			StopInteractItem(&InteractInstanceData);
		}
	}

	//如果状态有改变，需要发通知
	if (InteractState != InteractInstanceData.InteractState)
	{
		InteractInstanceData.NeedNofity = true;
	}

	//如果子状态有改变，需要发通知
	else if (InteractState == EInteractState::E_Potential && InteractInstanceData.SubPotentialIndex != SubPotentialIndex)
	{
		InteractInstanceData.NeedNofity = true;
	}

	InteractInstanceData.InteractState = InteractState;
	InteractInstanceData.SubPotentialIndex = SubPotentialIndex;
}

void UInteractSubsystem::RebuildInteractData(bool bForce)
{
	//如果没有交互组件，肯定不需要更新的
	if (InteractInstanceMap.Num() == 0)
	{
		return;
	}

	if (!bEnabled && !bForce)
	{
		return;
	}

	//删除不合法的数据
	for (auto& ObjectPtr : InvalidateObjects)
	{
		InteractInstanceMap.Remove(ObjectPtr);
	}
	InvalidateObjects.Empty();

	//没有交互对象，不需要计算了
	if (!InteractPawn.IsValid())
	{
		return;
	}

	//当前正在交互
	bool bInteracting = false;
	if (FInteractInstanceData* InteractingData = GetInteractingInstance())
	{
		//判断当前正在响应的是否还有效
		UpdateItem(*InteractingData);
		if (InteractingData->InteractState == EInteractState::E_Interacting)
		{
			bInteracting = true;

			//将所有激活的都关掉
			for (auto ActivateInstance : ActivateInstanceList)
			{
				if (ActivateInstance)
				{
					ActivateInstance->InteractState = EInteractState::E_None;
					NotifyInstanceList.Add(ActivateInstance);
				}
			}
		}
	}

	InteractiveInstanceList.Empty();
	ActivateInstanceList.Empty();

	//正在交互，不需要重新检测了
	if (!bInteracting)
	{
		//计算一遍位置数据
		for (auto& InstanceItem : InteractInstanceMap)
		{
			//没有交互信息的物体下一帧清除掉
			if (InstanceItem.Value.InstanceDataList.Num() == 0)
			{
				InvalidateObjects.Add(InstanceItem.Key);
				continue;
			}

			//更新交互物体信息
			for (int i = 0; i < InstanceItem.Value.InstanceDataList.Num(); i++)
			{
				FInteractInstanceData& InstanceData = InstanceItem.Value.InstanceDataList[i];
				UpdateItem(InstanceData);

				if (InstanceData.InteractState == EInteractState::E_Interactive)
				{
					InteractiveInstanceList.Add(&InstanceData);
				}
				else if (InstanceData.InteractState != EInteractState::E_None && InstanceData.InteractState != EInteractState::E_Interacting)
				{
					ActivateInstanceList.Add(&InstanceData);
				}

				if (InstanceData.NeedNofity)
				{
					NotifyInstanceList.Add(&InstanceData);
				}
			}
		}

		//排序
		SortItems();
		for (int i = 0; i < InteractiveInstanceList.Num(); i++)
		{
			if (InteractiveInstanceList[i]->InteractOrder != i)
			{
				InteractiveInstanceList[i]->InteractOrder = i;
				InteractiveInstanceList[i]->NeedNofity = true;
			}
		}
	}

	//通知
	for (auto& InstanceData : NotifyInstanceList)
	{
		UObject* ItemObject = InstanceData->ItemInterface.GetObject();
		if (InstanceData && ItemObject)
		{
			NotifyStatChange(ItemObject, *InstanceData);
			InstanceData->NeedNofity = false;
		}
	}
	NotifyInstanceList.Empty();
}

void UInteractSubsystem::SortItems()
{
	InteractiveInstanceList.Sort([](const FInteractInstanceData& A, const FInteractInstanceData& B)
		{
			//优先有响应的
			if (A.InteractState != B.InteractState)
			{
				return A.InteractState > B.InteractState;
			}

			//优先级排序
			if (A.ConfigData.Priority != B.ConfigData.Priority)
			{
				return A.ConfigData.Priority > B.ConfigData.Priority;
			}

			//距离
			if (A.Distance != B.Distance)
			{
				return A.Distance < B.Distance;
			}

			//角度
			return A.Angle > B.Angle;;
		});
}

void UInteractSubsystem::StopNowInteractingItem()
{
	if (FInteractInstanceData* InteractingData = GetInteractingInstance())
	{
		StopInteractItem(InteractingData); 
	}
}


void UInteractSubsystem::SetItemEnable(TScriptInterface<IInteractItemInterface> ItemInterface, const FName& ConfigName, bool Enable)
{
	FInteractInstanceData* InteractData = FindInstance(ItemInterface.GetObject(), ConfigName);
	if (InteractData)
	{
		InteractData->Enable = Enable;
	}
}


bool UInteractSubsystem::GetItemEnable(TScriptInterface<IInteractItemInterface> ItemInterface, const FName& ConfigName)
{
	FInteractInstanceData* InteractData = FindInstance(ItemInterface.GetObject(), ConfigName);
	if (InteractData)
	{
		return InteractData->Enable;
	}

	return false;
}

void UInteractSubsystem::SetItemEnableByInterface(TScriptInterface<IInteractItemInterface> ItemInterface, bool Enable)
{
	if (InteractInstanceMap.Num() == 0)
	{
		return;
	}

	FObjectInstanceDataInfo* ObjectInstanceDataInfoPtr = InteractInstanceMap.Find(ItemInterface.GetObject());
	if (ObjectInstanceDataInfoPtr)
	{
		for (int i = 0; i < ObjectInstanceDataInfoPtr->InstanceDataList.Num(); i++)
		{
			FInteractInstanceData& InstanceData = ObjectInstanceDataInfoPtr->InstanceDataList[i];
			if (InstanceData.ItemInterface == ItemInterface)
			{
				InstanceData.Enable = Enable;
			}
		}
	}
	
}

void UInteractSubsystem::StopInteractByInterface(TScriptInterface<IInteractItemInterface> ItemInterface)
{
	if (InteractInstanceMap.Num() == 0)
	{
		return;
	}

	FObjectInstanceDataInfo* ObjectInstanceDataInfoPtr = InteractInstanceMap.Find(ItemInterface.GetObject());
	if (ObjectInstanceDataInfoPtr)
	{
		for (int i = 0; i < ObjectInstanceDataInfoPtr->InstanceDataList.Num(); i++)
		{
			FInteractInstanceData& InstanceData = ObjectInstanceDataInfoPtr->InstanceDataList[i];
			if (InstanceData.ItemInterface == ItemInterface && InstanceData.InteractState == EInteractState::E_Interacting)
			{
				StopInteractItem(&InstanceData);
			}
		}
	}
}

void UInteractSubsystem::SetItemEnableByName(FName ConfigName, bool Enable)
{
	for (auto& InstanceItem : InteractInstanceMap)
	{
		for (auto& Instance : InstanceItem.Value.InstanceDataList)
		{
			if (Instance.ConfigData.ConfigName == ConfigName)
			{
				Instance.Enable = Enable;
			}
		}
	}
}

void UInteractSubsystem::ReceiveInput(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UInteractSubsystem::ReceiveInput error, Invalid Input Tag"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UInteractSubsystem::ReceiveInput %s"), *InputTag.ToString());


	int InteractIndex = 0;
	for (int i = 0; i < InteractiveInstanceList.Num(); i++)
	{
		FInteractInstanceData* InstanceData = InteractiveInstanceList[i];
		if (InstanceData && InstanceData->ConfigData.InputTag == InputTag && InstanceData->Enable)
		{
			if (InteractIndex != 0 && InstanceData->ConfigData.OnlyInHighest)
			{
				continue;
			}

			UObject* Object = InstanceData->ItemInterface.GetObject();
			if (UGameplayUtilsLibrary::IsValid(Object))
			{
				if (!IInteractItemInterface::Execute_CanInteract(Object, *InstanceData))
				{
					continue;
				}

				StartInteractItem(InstanceData);
				InteractIndex++;
			}
		}
	}
}

void UInteractSubsystem::StartInteractItem(FInteractInstanceData* InstanceData)
{
	if (InstanceData->ConfigData.InteractRole == EInteractRole::E_Client)
	{
		InternalStartInteractItem(InstanceData->ItemInterface, InteractPawn.Get(), InstanceData->ConfigData.ConfigName);
	}
	else
	{
		UInteractManagerComponent* ManagerComponent = nullptr;
		if (InteractPawn.IsValid())
		{
			ManagerComponent = Cast<UInteractManagerComponent>(InteractPawn->GetComponentByClass(UInteractManagerComponent::StaticClass()));
		}
		if (ManagerComponent)
		{
			ManagerComponent->ServerRequestStartInteract(InstanceData->ItemInterface, InteractPawn.Get(), InstanceData->ConfigData.ConfigName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UInteractSubsystem::StartInteractItem error, Cannot find Manager Component"));
		}
	}
}

void UInteractSubsystem::InternalStartInteractItem(TScriptInterface<IInteractItemInterface> InteractItem, APawn* Instigator, const FName& ConfigName)
{
	UObject* ItemObject = InteractItem.GetObject();
	FInteractInstanceData* InstanceData = FindInstance(ItemObject, ConfigName);
	if (InstanceData)
	{
		IInteractItemInterface::Execute_StartInteract(ItemObject, *InstanceData);
	}

	//主控端
	if (!bInServer && Instigator == InteractPawn)
	{
		SetInteracting(InstanceData);
		
		//通知
		NotifyStatChange(ItemObject, *InstanceData);
	}
}


void UInteractSubsystem::StopInteractItem(FInteractInstanceData* InstanceData)
{
	if (InstanceData->ConfigData.InteractRole == EInteractRole::E_Client)
	{
		InternalStopInteractItem(InstanceData->ItemInterface, InteractPawn.Get(), InstanceData->ConfigData.ConfigName);
	}
	else
	{
		UInteractManagerComponent* ManagerComponent = nullptr;
		if (InteractPawn.IsValid())
		{
			ManagerComponent = Cast<UInteractManagerComponent>(InteractPawn->GetComponentByClass(UInteractManagerComponent::StaticClass()));
		}
		ManagerComponent->ServerRequestStopInteract(InstanceData->ItemInterface, InteractPawn.Get(), InstanceData->ConfigData.ConfigName);
	}
}

void UInteractSubsystem::NotifyStatChange(UObject* Object, const FInteractInstanceData& InstanceData)
{
	if (Object)
	{
		IInteractItemInterface::Execute_OnInteractStateChange(Object, InstanceData);
	}
}

void UInteractSubsystem::InternalStopInteractItem(TScriptInterface<IInteractItemInterface> InteractItem, APawn* Instigator, const FName& ConfigName)
{
	UObject* ItemObject = InteractItem.GetObject();


	FInteractInstanceData* InstanceData = FindInstance(ItemObject, ConfigName);
	if (InstanceData)
	{
		IInteractItemInterface::Execute_EndInteract(ItemObject, *InstanceData);
	}

	if (!bInServer && Instigator == InteractPawn)
	{
		SetInteracting(nullptr);
		InstanceData->InteractState = EInteractState::E_None; //下一个tick会计算是否可交互
	}
}

void UInteractSubsystem::IgnoreVectorAxis(FVector& Vector, EInteractAngleAxis IgnoreAxis)
{
	if (IgnoreAxis == EInteractAngleAxis::E_X)
	{
		Vector.X = 0;
		Vector.Normalize();
	}
	else if (IgnoreAxis == EInteractAngleAxis::E_Y)
	{
		Vector.Y = 0;
		Vector.Normalize();
	}
	else if (IgnoreAxis == EInteractAngleAxis::E_Z)
	{
		Vector.Z = 0;
		Vector.Normalize();
	}
}

FInteractInstanceData* UInteractSubsystem::GetInteractingInstance()
{
	FInteractInstanceData* InteractingInst = FindInstance(InteractingObject.Get(), InteractingConfig);
	if (InteractingInst && InteractingInst->InteractState == EInteractState::E_Interacting)
	{
		return InteractingInst;
	}
	return nullptr;
}

void UInteractSubsystem::SetInteracting(FInteractInstanceData* InstanceData)
{
	if (!InstanceData)
	{
		InteractingObject = nullptr;
	}
	else
	{
		InteractingObject = InstanceData->ItemInterface.GetObject();
		InteractingConfig = InstanceData->ConfigData.ConfigName;
		InstanceData->InteractState = EInteractState::E_Interacting;
	}
}
