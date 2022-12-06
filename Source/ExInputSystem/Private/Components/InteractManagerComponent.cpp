// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ExGameplayLibrary.h"
#include "ExInputSubsystem.h"

UInteractManagerComponent::UInteractManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	//只在主控端上运行
	AActor* Owner = GetOwner();
	if (Owner && UExGameplayLibrary::IsClient(Owner))
	{
		SetComponentTickEnabled(true);
	}
	else
	{
		SetComponentTickEnabled(false);
	}
}

FInteractData* UInteractManagerComponent::FindItem(UInteractItemComponent* Component, const FGameplayTag& InteractType)
{
	for (int i = 0; i < InteractList.Num(); i++)
	{
		if (InteractList[i].InteractType == InteractType && InteractList[i].InteractItemComponent == Component)
		{
			return &InteractList[i];
		}
	}
	return nullptr;
}

FInteractData* UInteractManagerComponent::FindOrAddItem(UInteractItemComponent* Component, const FGameplayTag& InteractType)
{
	if (!Component)
	{
		return nullptr;
	}

	//查找
	FInteractData* InteractData = FindItem(Component, InteractType);
	if (InteractData)
	{
		return InteractData;
	}

	//添加
	InteractData = new(InteractList) FInteractData(GetOwner(), Component, InteractType);
	if (Component)
	{
		FInteractConfigData* ConfigData = Component->GetConfigData(InteractType);
		if (ConfigData)
		{
			InteractData->Enable = ConfigData->Enable;
		}
	}

	UpdateItem(*InteractData);

	return InteractData;
}

void UInteractManagerComponent::UpdateItem(FInteractData& InteractData)
{
	if (!InteractData.InteractItemComponent.IsValid())
	{
		return;
	}

	//非Enable的不用算
	if (!InteractData.Enable)
	{
		if (InteractData.InteractState != E_Interact_None)
		{
			InteractData.InteractState = E_Interact_None;
			InteractData.NeedNofity = true;
		}
		return;
	}

	//找到配置数据
	const FInteractConfigData* ConfigData = InteractData.InteractItemComponent->GetConfigData(InteractData.InteractType);
	if (!ConfigData)
	{
		InteractData.InteractState = E_Interact_None;
		return;
	}

	//玩家和物品的变换
	AActor* OwnerActor = GetOwner();
	AActor* ItemActor = InteractData.InteractItemComponent->GetOwner();
	FTransform PawnTransform = OwnerActor->GetTransform();
	FTransform ItemTransform = InteractData.InteractItemComponent->GetOwner()->GetTransform();

	//玩家交互方向
	FVector PawnForward = OwnerActor->GetActorForwardVector();
	if (!PawnForward.IsNearlyZero())
	{
		PawnForward = UKismetMathLibrary::TransformDirection(PawnTransform, InteractRange.Direction);
	}

	//物品相对于玩家的方向, 世界坐标
	FVector ItemLocation = ItemActor->GetActorLocation() + ConfigData->CenterOffset;
	FVector ItemDirection = (ItemLocation - OwnerActor->GetActorLocation());

	//物品相对玩家的距离
	InteractData.Distance = ItemDirection.Size();

	//物品与玩家相对玩家交互方向的夹角cos值
	ItemDirection.Normalize();
	InteractData.Angle = FVector::DotProduct(PawnForward, ItemDirection);


	//1. 先按距离做判断
	EInteractState InteractState = E_Interact_None;
	if (InteractData.Distance <= ConfigData->InteractiveRadius)
	{
		InteractState = E_Interact_Interactive;
	}
	else if (InteractData.Distance <= ConfigData->PotentialRadius)
	{
		InteractState = E_Interact_Potential;
	}
	else
	{
		InteractState = E_Interact_None;
	}

	//2. 是否在manager的可交互角度范围内
	if (InteractState == E_Interact_Interactive && InteractRange.Angle > 0)
	{
		//物体配置朝向和玩家对于物体的方向
		FVector ItemToPawnLocalDirection = UKismetMathLibrary::InverseTransformDirection(PawnTransform, ItemDirection);
		FVector PawnForwardLocalDirction = UKismetMathLibrary::InverseTransformDirection(PawnTransform, PawnForward);

		IgnoreVectorAxis(ItemToPawnLocalDirection, InteractRange.IgnoreAxis);
		IgnoreVectorAxis(PawnForwardLocalDirction, InteractRange.IgnoreAxis);

		float ItemCosAngle = FVector::DotProduct(ItemToPawnLocalDirection, PawnForwardLocalDirction);
		if (ItemCosAngle < FMath::Cos(InteractRange.Angle * PI / 180))
		{
			InteractState = E_Interact_Potential;
		}
	}

	//3. 是否在Item的可交互范围内
	if (InteractState == E_Interact_Interactive && ConfigData->Ranges.Num() > 0)
	{
		bool HasFound = false;
		for (int i = 0; i < ConfigData->Ranges.Num(); i++)
		{
			const FInteractRange& ItemRange = ConfigData->Ranges[i];
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
			InteractState = E_Interact_Potential;
		}
	}

	//如果状态有改变，需要重新排序
	if (InteractState != InteractData.InteractState)
	{
		InteractData.NeedNofity = true;
	}
	InteractData.InteractState = InteractState;
}

void UInteractManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (InteractList.Num() == 0)
	{
		return;
	}

	//删除不合法的数据
	for (int i = InteractList.Num() - 1; i >= 0; i--)
	{
		if (!InteractList[i].InteractItemComponent.IsValid())
		{
			InteractList.RemoveAt(i);
		}
	}

	//计算一遍位置数据，预备排序
	InteractOrderList.Empty();
	for (int i = 0; i < InteractList.Num(); i++)
	{
		UpdateItem(InteractList[i]);

		if (InteractList[i].InteractState == E_Interact_Interactive)
		{
			InteractOrderList.Add(&InteractList[i]);
		}
	}

	//排序
	SortItems();
	for (int i = 0; i < InteractOrderList.Num(); i++)
	{
		if (InteractOrderList[i]->InteractOrder != i)
		{
			InteractOrderList[i]->InteractOrder = i;
			InteractOrderList[i]->NeedNofity = true;
		}
	}

	//通知
	for (int i = 0; i < InteractList.Num(); i++)
	{
		FInteractData& InteractData = InteractList[i];
		if (InteractData.NeedNofity)
		{
			InteractData.InteractItemComponent->NotifyInteractStateChange(InteractData);
			InteractData.NeedNofity = false;
		}
	}
}

void UInteractManagerComponent::SortItems()
{
	InteractOrderList.Sort([](const FInteractData& A, const FInteractData& B)
		{
			const FInteractConfigData* AConfig = A.InteractItemComponent != nullptr ? A.InteractItemComponent->GetConfigData(A.InteractType) : nullptr;
			const FInteractConfigData* BConfig = B.InteractItemComponent != nullptr ? B.InteractItemComponent->GetConfigData(B.InteractType) : nullptr;
			if (!AConfig || !BConfig)
			{
				if (AConfig) return true;
				else return false;
			}

			//优先有响应的
			if (A.InteractState != B.InteractState)
			{
				return A.InteractState > B.InteractState;
			}

			//优先级排序
			if (AConfig->Priority != BConfig->Priority)
			{
				return AConfig->Priority > BConfig->Priority;
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

void UInteractManagerComponent::SetEnable(bool Enable)
{
	PrimaryComponentTick.bCanEverTick = Enable;
}

bool UInteractManagerComponent::GetEnable()
{
	return PrimaryComponentTick.bCanEverTick;
}

void UInteractManagerComponent::SetItemEnable(UInteractItemComponent* Component, const FGameplayTag& InteractType, bool Enable)
{
	if (InteractType.IsValid())
	{
		FInteractData* InteractData = FindItem(Component, InteractType);
		if (InteractData)
		{
			InteractData->Enable = Enable;
		}
	}
	else
	{
		for (int i = 0; i < InteractList.Num(); i++)
		{
			if (InteractList[i].InteractItemComponent == Component)
			{
				InteractList[i].Enable = Enable;
			}
		}
	}
}

bool UInteractManagerComponent::GetItemEnable(UInteractItemComponent* Component, const FGameplayTag& InteractType)
{
	FInteractData* InteractData = FindItem(Component, InteractType);
	if (InteractData)
	{
		return InteractData->Enable;
	}

	return false;
}

void UInteractManagerComponent::SetComponentEnable(UInteractItemComponent* Component, bool Enable)
{
	for (int i = 0; i < InteractList.Num(); i++)
	{
		if (InteractList[i].InteractItemComponent == Component)
		{
			InteractList[i].Enable = Enable;
		}
	}
}

bool UInteractManagerComponent::ReceiveInput(const FGameplayTag& InputTag)
{
	UExInputSubsystem * InputSubsystem = UExInputSubsystem::GetInputSubsystem(this);
	bool IsHandled = InputSubsystem->HandleInputEvent(InputTag).IsHandled;
	if (IsHandled)
	{
		return true;
	}

	for (int i = 0; i < InteractOrderList.Num(); i++)
	{
		FInteractData* InteractData = InteractOrderList[i];
		FInteractConfigData* ConfigData = InteractData->InteractItemComponent->GetConfigData(InteractData->InteractType);

		if (ConfigData && ConfigData->InteractTag == InputTag && ConfigData->Enable)
		{
			if (i != 0 && ConfigData->OnlyInHighest)
			{
				continue;
			}

			//只在客户端允许的交互是有返回结果的
			if (ConfigData->InteractRole == E_Interact_Role_OnlyClient)
			{
				IsHandled = InternalInteractItem(*InteractData) ? true:IsHandled;
			}
			//要经过DS确认的交互，因为是异步的，没法确认返回结果
			else
			{
				ServerInteractItem(*InteractData);
			}
		}
	}

	if (!IsHandled)
	{
		IsHandled = BP_ReceiveInput(InputTag);
	}

	return IsHandled;
}

bool UInteractManagerComponent::BP_ReceiveInput_Implementation(const FGameplayTag& InputTag)
{
	return false;
}

void UInteractManagerComponent::ServerInteractItem_Implementation(const FInteractData& InteractData)
{
	bool result = InternalInteractItem(InteractData);
	if (result)
	{
		ClientInteractItem(InteractData);
	}
}

bool UInteractManagerComponent::ServerInteractItem_Validate(const FInteractData& InteractData)
{
	return true;
}

void UInteractManagerComponent::ClientInteractItem_Implementation(const FInteractData& InteractData)
{
	InternalInteractItem(InteractData);
}

bool UInteractManagerComponent::ClientInteractItem_Validate(const FInteractData& InteractData)
{
	return true;
}

bool UInteractManagerComponent::InternalInteractItem(const FInteractData& InteractData)
{
	return InteractData.InteractItemComponent->OnInteract(InteractData);
}

void UInteractManagerComponent::IgnoreVectorAxis(FVector& Vector, EInteractAngleAxis IgnoreAxis)
{
	if (IgnoreAxis == E_Interact_Angle_X)
	{
		Vector.X = 0;
		Vector.Normalize();
	}
	else if (IgnoreAxis == E_Interact_Angle_Y)
	{
		Vector.Y = 0;
		Vector.Normalize();
	}
	else if (IgnoreAxis == E_Interact_Angle_Z)
	{
		Vector.Z = 0;
		Vector.Normalize();
	}
}
