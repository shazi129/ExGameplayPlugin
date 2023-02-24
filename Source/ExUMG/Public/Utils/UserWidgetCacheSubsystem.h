#pragma once 

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UserWidgetCacheSubsystem.generated.h"

//缓存类型
UENUM(BlueprintType)
enum EUserWidgetCacheType
{
	E_NO_CACHE = 0				   UMETA(DisplayName = "No Cache"),  //没有缓存
	E_FOREVER_CACHE               UMETA(DisplayName = "Forever"), //永久缓存
};

UCLASS(BlueprintType)
class EXUMG_API UUserWidgetCacheSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void OnWorldTearingDown(UWorld* World);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UUserWidgetCacheSubsystem* GetSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	UUserWidget* FindOrCreateUserWidget(TSubclassOf<UUserWidget> WidgetClass, EUserWidgetCacheType CacheType = E_FOREVER_CACHE);

	UFUNCTION(BlueprintCallable)
	UUserWidget* FindOrCreateUserWidgetByPath(const FString& UIObjectPath, EUserWidgetCacheType CacheType = E_FOREVER_CACHE);

	UFUNCTION(BlueprintCallable)
	void RemoveAll();

private:
	UUserWidget* GetCachedWidget(FName WidgetName);

	UPROPERTY()
	TMap<FName, UUserWidget*> WidgetCache;
};