﻿#pragma once

#include "CoreMinimal.h"
#include "ReplicatedSubObject.generated.h"

/**
 * @brief 用于给Actor添加一个可复制的子对象
 * 用法示例：
 * class AMyActor: public Actor
 * {
 * public:
 *		AMyActor::AMyActor()
 *		{
 *			//Actor本身可复制
 *			bReplicates = true;
 *			bReplicateUsingRegisteredSubObjectList = true;
 *		}
 * 
 *		//定义可复制子对象
 *		UPROPERTY(BlueprintReadOnly, Replicated)
 *		TObjectPtr<UReplicatedSubObject> SubObject;
 * 
 *		//服务端创建子对象
 *		virtual void BeginPlay() override
 *		{
 *			Super::BeginPlay();
 *			if (HasAuthority())
 *			{
 *				SubObject = NewObject<UReplicatedSubObject>(this, UReplicatedSubObject::StaticClass());
 *				AddReplicatedSubObject(SubObject);
 *			}
 *		}
 * 
 *		//复制到Client
 *		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
 *		{
 *			Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 *			DOREPLIFETIME_CONDITION(AMyActor, SubObject, COND_None);
 *		}
 * }
*/

UCLASS()
class GAMEPLAYUTILS_API UReplicatedSubObject : public UObject
{
	GENERATED_BODY()

public:
	UReplicatedSubObject();

	virtual bool IsNameStableForNetworking() const override;
	virtual bool IsSupportedForNetworking() const override;
	virtual void PreNetReceive() override;
	virtual void PostNetReceive() override;

};
