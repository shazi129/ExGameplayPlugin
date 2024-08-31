#pragma once 

/**
 * 游戏中与场景相关，比较通用的一些WorldSubsystem功能
 */

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectMulticastDelegate, UObject*, Object);
DECLARE_DYNAMIC_DELEGATE_OneParam(FObjectDynamicDelegate, UObject*, Object);

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FGameplayURL
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Protocol;

	UPROPERTY(BlueprintReadWrite)
	FString Host;

	UPROPERTY(BlueprintReadWrite)
	int32 Port;

	UPROPERTY(BlueprintReadWrite)
	FString Map;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FString> OptionMap;

	void Parse(FURL URL);
};

UCLASS(BlueprintType)
class GAMEPLAYUTILS_API UGameplayWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UGameplayWorldSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

#pragma region //////////////////////////////////全局的一些Object缓存////////
public:
	UFUNCTION(BlueprintCallable)
	UObject* GetGlobalObject(FName ObjectName);

	UFUNCTION(BlueprintCallable)
	void GetGlobalObjectForDelegate(FName ObjectName, FObjectDynamicDelegate Delgate);

	UFUNCTION(BlueprintCallable)
	TArray<UObject*> GetGlobalObjectList(FName ObjectName);

	UFUNCTION(BlueprintCallable)
	TArray<UObject*> GetGlobalObjectListByClass(UClass* ObjectClass);

	UFUNCTION(BlueprintCallable)
	bool AddGlobalObject(FName ObjectName, UObject* Object);

	UFUNCTION(BlueprintCallable)
	void RemoveGlobalObjects(FName ObjectName);

private:
	//全局的Object
	UPROPERTY()
	TMap<FName, FObjectListData> GlobalObjectsMap;

	UPROPERTY()
	TMap<FName, FObjectMulticastDelegate> GlobalObjectGetDelegateMap;
#pragma endregion

public:
	UFUNCTION(BlueprintCallable)
	FGameplayURL GetRemoteURL();

private:
	FGameplayURL RemoteURL;
};