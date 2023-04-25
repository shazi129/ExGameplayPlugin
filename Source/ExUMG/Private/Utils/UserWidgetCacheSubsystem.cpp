#include "Utils/UserWidgetCacheSubsystem.h"
#include "ExUMGModule.h"
#include "ExMacros.h"
#include "Kismet/GameplayStatics.h"

UUserWidgetCacheSubsystem* UUserWidgetCacheSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogExUMG, UUserWidgetCacheSubsystem, WorldContextObject);
}

void UUserWidgetCacheSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UUserWidgetCacheSubsystem::OnWorldTearingDown);
}

void UUserWidgetCacheSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UUserWidgetCacheSubsystem::OnWorldTearingDown(UWorld* World)
{
	RemoveAll();
}


UUserWidget* UUserWidgetCacheSubsystem::GetCachedWidget(FName WidgetName)
{
	UUserWidget* ResultWidget = nullptr;

	if (WidgetCache.Contains(FName(WidgetName)))
	{
		ResultWidget = WidgetCache[WidgetName];

		//可能存在在外部被干掉的情况
		if (ResultWidget == nullptr || ResultWidget->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed))
		{
			WidgetCache.Remove(WidgetName);
			ResultWidget = nullptr;
		}
	}
	return ResultWidget;
}


UUserWidget* UUserWidgetCacheSubsystem::FindOrCreateUserWidgetByPath(const FString& UIObjectPath, EUserWidgetCacheType CacheType)
{
	if (UUserWidget* CachedWidget = GetCachedWidget(FName(UIObjectPath)))
	{
		EXUMG_LOG(Log, TEXT("%s find in cache, UIObjectPath[%s], CacheType[%d]"), *FString(__FUNCTION__), *UIObjectPath, CacheType);
		return CachedWidget;
	}

	UClass* WidgetCls = LoadClass<UUserWidget>(this, *UIObjectPath);
	if (WidgetCls == nullptr)
	{
		EXUMG_LOG(Error, TEXT("%s error, Load class path[%s] error "), *FString(__FUNCTION__), *UIObjectPath);
		return nullptr;
	}

	return FindOrCreateUserWidget(WidgetCls, CacheType);
}

UUserWidget* UUserWidgetCacheSubsystem::FindOrCreateUserWidget(TSubclassOf<UUserWidget> WidgetClass, EUserWidgetCacheType CacheType)
{
	if (WidgetClass == nullptr)
	{
		EXUMG_LOG(Error, TEXT("%s error, WidgetClass is null"), *FString(__FUNCTION__));
		return nullptr;
	}
	
	FName WidgetClassName = FName(FString::Printf(TEXT("%s.%s"), *WidgetClass->GetOuter()->GetName(), *WidgetClass->GetName()));	
	if (UUserWidget* CachedWidget = GetCachedWidget(WidgetClassName))
	{
		EXUMG_LOG(Log, TEXT("%s find in cache, WidgetClassName[%s], CacheType[%d]"), *FString(__FUNCTION__), *WidgetClassName.ToString(), CacheType);
		return CachedWidget;
	}

	//如果没找到，创建
	EXUMG_LOG(Log, TEXT("%s, WidgetClassName[%s], CacheType[%d]"), *FString(__FUNCTION__), *WidgetClassName.ToString(), CacheType);
	UUserWidget *ResultWidget = CreateWidget(GetWorld(), WidgetClass);
	if (CacheType == EUserWidgetCacheType::E_FOREVER_CACHE)
	{
		WidgetCache.Add(WidgetClassName, ResultWidget);
	}
	return ResultWidget;

}

void UUserWidgetCacheSubsystem::RemoveAll()
{
	WidgetCache.Reset();
}
