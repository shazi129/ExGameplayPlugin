﻿#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "GameplayTypes.h"
#include "GameplayUtilsLibrary.generated.h"

UCLASS()
class GAMEPLAYUTILS_API UGameplayUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 执行一个命令
	 * @param Command 要执行的命令
	 * @return 是否执行成功
	*/
	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static bool ExecCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static bool FilterActorClasses(AActor* Actor, const TArray<TSubclassOf<AActor>>& ActorClasses);

	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static void FilterActors(const TArray<AActor*>& Actors, const FFilterActorCondition FilterCondition, TArray<AActor*>& OutActors);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool CheckExecNetMode(const UObject* WorldContextObject, EExecNetMode ExecNetMode);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static bool IsLocalControled(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static UObject* GetDefaultObject(UClass* ObjectClass);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static APawn* GetPawnByPlayerState(APlayerState* PlayerState);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static APlayerState* GetPlayerStateByActor(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static UActorComponent* GetComponentByTag(AActor* Actor, const FName& Tag);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static UActorComponent* GetComponentByClass(AActor* Actor, TSubclassOf<UActorComponent> ComponentClass);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static FString GetNameSafe(const UObject* Object);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
		static FString GetContextWorldName(const UObject* Object);

	/**
	 * @brief 复制一个UObject
	 * @param TemplateObject 复制模板
	 * @param Outer 新Object的Outer
	 * @param Name 新Object的Name
	 * @return 新Object
	*/
	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static UObject* CopyObject(UObject* TemplateObject, UObject* Outer, FName Name = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static TArray<UActorComponent*> GetComponentsByTickEnable(AActor* Actor, bool Enabled);

	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static void SetComponentsTickEnable(TArray<UActorComponent*>& Components, bool Enable);

	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
		static bool IsSameWorld(const UWorld* World, const TSoftObjectPtr<UWorld>& TargetWorld);

	/**
	 * @brief 判断当前World是否是指定的World
	 * @param WorldContextObject 上下文Object
	 * @param TargetWorld 指定的World
	 * @return 结果
	*/
	UFUNCTION(BlueprintPure, Category = "GameplayUtils", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool IsCurrentWorld(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld>& TargetWorld);

	/**
	 * @brief 判断Wold是否是一个有效的GameWorld
	 * @param WorldContextObject 
	 * @return 
	*/
	UFUNCTION(BlueprintPure, Category = "GameplayUtils", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool IsGameWorld(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
	static FTransform GetBoneTransform(USkinnedMeshComponent* SkinnedMeshComponent, int32 BoneIndex);

	/**
	 * @brief 根据物理骨骼名获取骨骼位置
	 * @param SkeletalMeshComponent 
	 * @param BoneNames 
	 * @return 
	*/
	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
	static TMap<FName, FTransform> GetBonesTransform(USkeletalMeshComponent* SkeletalMeshComponent, const TArray<FName>& BoneNames);

	/**
	 * @brief 根据组件名获取组件
	 * @param Actor 
	 * @param ComponentName 
	 * @return 
	*/
	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
	static UActorComponent* GetComponentByName(AActor* Actor, const FString& ComponentName, UClass* CompClass=nullptr);

	/**
	 * @brief 根据权重配置随机获取道具
	 * @param WorldContextObject 
	 * @param ItemWeightsEntryList 权重配置
	 * @param ItemCount 要获取的道具数量
	 * @return 得到的ID列表
	*/
	UFUNCTION(BlueprintPure, Category = "GameplayUtils", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static TArray<int32> RandomItemsByWeight(const UObject* WorldContextObject, const TArray<FItemWeightsEntry>& ItemWeightsEntryList, int32 ItemCount=1);

	/**
	 * @brief 设置是否响应物理碰撞，即"Simulation Generates Hit Events"字段
	 * @param Component 
	 * @param bNewNotifyCollision 
	*/
	UFUNCTION(BlueprintCallable, Category = "GameplayUtils")
	static void SetInstanceNotifyRBCollision(UPrimitiveComponent* Component, bool bNewNotifyCollision);

	/**
	 * @brief 判断一个Object是否合法
	 * @param Object 
	 * @return 
	*/
	UFUNCTION(BlueprintPure, Category = "GameplayUtils")
	static bool IsValid(UObject* Object);
};