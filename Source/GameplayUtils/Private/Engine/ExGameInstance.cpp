#include "Engine/ExGameInstance.h"
#include "Settings/GameplayUtilSettings.h"

TSubclassOf<UOnlineSession> UExGameInstance::GetOnlineSessionClass()
{
	TSoftClassPtr<UOnlineSession> ClassPtr = GetDefault<UGameplayUtilSettings>()->OnLineSessionClass;
	UClass* SessionClass = ClassPtr.LoadSynchronous();
	if (!SessionClass)
	{
		SessionClass = Super::GetOnlineSessionClass();
	}
	return SessionClass;
}
