#include "Settings/GameplayUtilSettingSubsystem.h"
#include "Settings/GameplayUtilSettings.h"
#include "Kismet/GameplayStatics.h"

void UGameplayUtilSettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitGlobalVariables();

	for (auto& ObjectPtr : GetDefault<UGameplayUtilSettings>()->PermanentObjects)
	{
		if (!ObjectPtr.IsNull())
		{
			PermanentObjects.Add(ObjectPtr.LoadSynchronous());
		}
	}
}

void UGameplayUtilSettingSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UGameplayUtilSettingSubsystem::InitGlobalVariables()
{
}
