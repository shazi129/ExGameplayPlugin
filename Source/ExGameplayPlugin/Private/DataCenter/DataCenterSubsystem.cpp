#include "DataCenter/DataCenterSubsystem.h"
#include "Kismet/BlueprintPathsLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ExGameplayPluginModule.h"

UDataCenterSubsystem* UDataCenterSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UDataCenterSubsystem>();
}

FString UDataCenterSubsystem::GetDefaultDataPath()
{
	return  UKismetSystemLibrary::ConvertToAbsolutePath(UBlueprintPathsLibrary::ProjectContentDir() + "Data/DataTable/");
}

FString UDataCenterSubsystem::GetCacheDataPath(int ZoneID, const FString& Environment)
{
	return  UKismetSystemLibrary::ConvertToAbsolutePath(FString::Printf(TEXT("%sData/DataTable/%s/%d/"), *UBlueprintPathsLibrary::ProjectSavedDir(), *Environment, ZoneID));
}

void UDataCenterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);	
}

void UDataCenterSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UDataCenterSubsystem::RebuildDescriptor(int ZoneID, const FString& Environment)
{
	EXGAMEPLAY_LOG(Log, TEXT("%s ZoneID[%d], Enviroment[%s]"), *FString(__FUNCTION__), ZoneID, *Environment);

	FText FailureReason;

	//默认数据，打进包里的那份
	FString DefaultDataPath = UDataCenterSubsystem::GetDefaultDataPath() + "Version";
	if (!CurrentDescriptor.LoadFromFile(DefaultDataPath, FailureReason))
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s, cannot load default version file[%s], Error[%s]"), *FString(__FUNCTION__), *DefaultDataPath, *FailureReason.ToString());
	}
	if (ZoneID < 0 || Environment.IsEmpty())
	{
		return;	
	}

	FDataVersionDescriptor CachedDescriptior;
	FString CachedDataPath = UDataCenterSubsystem::GetCacheDataPath(ZoneID, Environment) + "Version";
	if (!CachedDescriptior.LoadFromFile(CachedDataPath, FailureReason))
	{
		EXGAMEPLAY_LOG(Log, TEXT("%s, cannot load Cache version file[%s], Error[%s]"), *FString(__FUNCTION__), *CachedDataPath, *FailureReason.ToString());
	}

	//看是否需要用cache里的数据
	if (CachedDescriptior.IsValid() && CachedDescriptior.Environment == Environment)
	{
		if (!CurrentDescriptor.IsValid() || CurrentDescriptor.Environment != Environment || CachedDescriptior > CurrentDescriptor)
		{
			EXGAMEPLAY_LOG(Log, TEXT("%s, use cache data replace default data"), *FString(__FUNCTION__));
			CurrentDescriptor = CachedDescriptior;
		}
	}

	//检查线上的配置
	for (const FDataVersionDescriptor& Descriptor : OnlineDescriptors)
	{
		if (Descriptor.IsValid() && Descriptor.Environment == Environment)
		{
			if (!CurrentDescriptor.IsValid() || CurrentDescriptor.Environment != Environment || Descriptor > CurrentDescriptor)
			{
				EXGAMEPLAY_LOG(Log, TEXT("%s, use online data replace default data"), *FString(__FUNCTION__));
				CurrentDescriptor = Descriptor;
			}
		}
	}

	EXGAMEPLAY_LOG(Log, TEXT("%s rebuild end, current desctiptor: %s"), *FString(__FUNCTION__), *CurrentDescriptor.ToString());
}


FString UDataCenterSubsystem::GetDataPath()
{
	if (CurrentDescriptor.IsValid())
	{
		return CurrentDescriptor.DataPath;
	}
	EXGAMEPLAY_LOG(Warning, TEXT("%s error, return default path"), *FString(__FUNCTION__));
	return UDataCenterSubsystem::GetDefaultDataPath();
}

void UDataCenterSubsystem::AddOnLineDesriptorWithJson(const FString& JsonContent)
{
	FDataVersionDescriptor* Descriptor = new (OnlineDescriptors)FDataVersionDescriptor();

	FText FailureReason;
	if (!Descriptor->LoadFromJson(JsonContent, FailureReason))
	{
		EXGAMEPLAY_LOG(Error, TEXT("%s Load Content error[%s],  Content[%s]"), *FString(__FUNCTION__), *JsonContent, *FailureReason.ToString());
		OnlineDescriptors.RemoveAt(OnlineDescriptors.Num() - 1);
	}
}

void UDataCenterSubsystem::AddOnLineDescriptor(const FDataVersionDescriptor& Descriptor)
{
	if (Descriptor.IsValid())
	{
		OnlineDescriptors.Add(Descriptor);
	}
}