#include "ExGameplayLibrary.h"
#include "ExGameplayLibraryModule.h"
#include "Misc/DateTime.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"

bool UExGameplayLibrary::IsClient(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		EXLIBRARY_LOG(Error, TEXT("UExGameplayLibrary::IsClient error, WorldContextObject is null"));
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return false;
	}

	ENetMode NetMode = World->GetNetMode();
	if (NetMode == ENetMode::NM_Standalone || NetMode == ENetMode::NM_Client)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int64 UExGameplayLibrary::GetTimestamp()
{
	return FDateTime::Now().ToUnixTimestamp();
}

EBPNetMode UExGameplayLibrary::GetWorldNetMode(UWorld* World)
{
	return (EBPNetMode)World->GetNetMode();
}

bool UExGameplayLibrary::IsEditorMode()
{
#if UE_EDITOR
	return true;
#else
	return false;
#endif
}

FString UExGameplayLibrary::FormatLeftTime(int64 Seconds, int Segments, FString FromatString)
{
	if (Seconds < 0)
	{
		Seconds = 0;
	}

	if (FromatString.IsEmpty())
	{
		FromatString = "%02d";
		for (int i = 1; i < Segments; i++)
		{
			FromatString = FromatString + ":%02d";
		}
	}
	TArray<FStringFormatArg> TimeValues;

	//��Ҫ��ʾ��
	if (Segments > 3)
	{
		int DaySeconds = 3600 * 24;
		TimeValues.Add(int(Seconds / DaySeconds));
		Seconds = Seconds % DaySeconds;
	}

	//��Ҫ��ʾСʱ
	if (Segments > 2)
	{
		int HourSeconds = 3600;
		TimeValues.Add(Seconds / HourSeconds);
		Seconds = Seconds % HourSeconds;
	}

	//��Ҫ��ʾ��
	if (Segments > 1)
	{
		int MinuteSeconds = 60;
		TimeValues.Add(Seconds / MinuteSeconds);
		Seconds = Seconds % MinuteSeconds;
	}

	if (Segments > 0) //��Ҫ��ʾ��
	{
		TimeValues.Add(Seconds);
	}
	return FString::Format(*FromatString, TimeValues);
}

bool UExGameplayLibrary::ExecCommand(const FString& Command)
{
	if (!GEngine)
	{
		UE_LOG(LogExGameplayLibrary, Error, TEXT("UExGameplayLibrary::ExecCommand error, GEngine is Null"));
		return false;
	}

	UWorld* World = nullptr;

	ULocalPlayer* Player = GEngine->GetDebugLocalPlayer();
	if (Player)
	{
		World = Player->GetWorld();
		if (World)
		{
			if (Player->Exec(World, *Command, *GLog))
			{
				return true;
			}
		}
		else
		{
			UE_LOG(LogExGameplayLibrary, Error, TEXT("UExGameplayLibrary::ExecCommand error, PlayerWorld is Null"));
			return false;
		}
	}

	if (!World)
	{
		World = GEngine->GetWorld();
	}
	return GEngine->Exec(World, *Command);
}

FString UExGameplayLibrary::GetLogPrefix(const UObject* WorldContextObject)
{
	if (WorldContextObject == nullptr)
	{
		return FString::Printf(TEXT("None World"));
	}

	UWorld* World = WorldContextObject->GetWorld();
	switch (World->GetNetMode())
	{
	case NM_Client:
		if (World->WorldType == EWorldType::PIE)
		{
			return FString::Printf(TEXT("Client %d"), GPlayInEditorID);
		}
		else
		{
			return FString::Printf(TEXT("Client"));
		}
		break;
	case NM_DedicatedServer:
	case NM_ListenServer:
		return FString::Printf(TEXT("Server"));
		break;
	case NM_Standalone:
		return FString::Printf(TEXT("Standalone"));
		break;
	}
	return FString::Printf(TEXT("Unsuppored"));
}

void UExGameplayLibrary::Log(const UObject* WorldContextObject, FString Message, EBPLogLevel Level)
{
	FString Prefix = GetLogPrefix(WorldContextObject);
	switch (Level)
	{
	case EBPLogLevel::Error:
		UE_LOG(LogTemp, Error, TEXT("[%s] %s"), *Prefix, *Message);
		break;
	case EBPLogLevel::Log:
		UE_LOG(LogTemp, Log, TEXT("[%s] %s"), *Prefix, *Message);
		break;
	case EBPLogLevel::Warning:
		UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), *Prefix, *Message);
		break;
	default:
		break;
	}
}

void UExGameplayLibrary::SetLumenEnable(const UObject* WorldContextObject, bool Enable)
{
	TArray<AActor*> PostProcessVolumeActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APostProcessVolume::StaticClass(), PostProcessVolumeActors);
	for (int i = 0; i < PostProcessVolumeActors.Num(); i++)
	{
		APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(PostProcessVolumeActors[i]);
		if (PostProcessVolume)
		{
			if (Enable)
			{
				PostProcessVolume->Settings.ReflectionMethod = EReflectionMethod::Type::Lumen;
				PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Type::Lumen;
			}
			else
			{
				PostProcessVolume->Settings.ReflectionMethod = EReflectionMethod::Type::None;
				PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Type::None;
			}
		}
	}
}

FGameplayTag UExGameplayLibrary::RequestGameplayTag(FName TagName, bool ErrorIfNotFound)
{
	return UGameplayTagsManager::Get().RequestGameplayTag(TagName, ErrorIfNotFound);
}

void UExGameplayLibrary::SetDynamicGlobalIlluminationLevel(const UObject* WorldContextObject, int Level)
{
	TArray<AActor*> PostProcessVolumeActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APostProcessVolume::StaticClass(), PostProcessVolumeActors);
	for (int i = 0; i < PostProcessVolumeActors.Num(); i++)
	{
		APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(PostProcessVolumeActors[i]);
		if (PostProcessVolume)
		{
			if (0 == Level)
			{
				PostProcessVolume->Settings.ReflectionMethod = EReflectionMethod::Type::Lumen;
				PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Type::Lumen;
			}
			else if(1 == Level)
			{
				PostProcessVolume->Settings.ReflectionMethod = EReflectionMethod::Type::ScreenSpace;
				PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Type::ScreenSpace;
				PostProcessVolume->Settings.AmbientOcclusionIntensity = 1;
				PostProcessVolume->Settings.AmbientOcclusionQuality = 100;
			}
			else
			{
				PostProcessVolume->Settings.ReflectionMethod = EReflectionMethod::Type::None;
				PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Type::None;
			}
		}
	}
}

void UExGameplayLibrary::SetPostProcessingGI(const UObject* WorldContextObject, int Level)
{
	TArray<AActor*> PostProcessVolumeActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APostProcessVolume::StaticClass(), PostProcessVolumeActors);
	for (int i = 0; i < PostProcessVolumeActors.Num(); i++)
	{
		APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(PostProcessVolumeActors[i]);
		if (PostProcessVolume)
		{
			if (0 == Level)
			{
				PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Type::Lumen;
			}
			else if(1 == Level)
			{
				PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Type::ScreenSpace;
			}
			else
			{
				PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Type::None;
			}
		}
	}
}

void UExGameplayLibrary::SetPostProcessingReflection(const UObject* WorldContextObject, int Level)
{
	TArray<AActor*> PostProcessVolumeActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APostProcessVolume::StaticClass(), PostProcessVolumeActors);
	for (int i = 0; i < PostProcessVolumeActors.Num(); i++)
	{
		APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(PostProcessVolumeActors[i]);
		if (PostProcessVolume)
		{
			if (0 == Level)
			{
				PostProcessVolume->Settings.ReflectionMethod = EReflectionMethod::Type::Lumen;
			}
			else if(1 == Level)
			{
				PostProcessVolume->Settings.ReflectionMethod = EReflectionMethod::Type::ScreenSpace;
			}
			else
			{
				PostProcessVolume->Settings.ReflectionMethod = EReflectionMethod::Type::None;
			}
		}
	}
}

void UExGameplayLibrary::SetPostProcessingSSAO(const UObject* WorldContextObject, int Level)
{
	TArray<AActor*> PostProcessVolumeActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APostProcessVolume::StaticClass(), PostProcessVolumeActors);
	for (int i = 0; i < PostProcessVolumeActors.Num(); i++)
	{
		APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(PostProcessVolumeActors[i]);
		if (PostProcessVolume)
		{
			if (0 == Level)
			{
				PostProcessVolume->Settings.AmbientOcclusionIntensity = 0;
				PostProcessVolume->Settings.AmbientOcclusionQuality = 0;
			}
			else if(1 == Level)
			{
				PostProcessVolume->Settings.AmbientOcclusionIntensity = 1;
				PostProcessVolume->Settings.AmbientOcclusionQuality = 100;
			}
			else
			{
				PostProcessVolume->Settings.AmbientOcclusionIntensity = 0;
				PostProcessVolume->Settings.AmbientOcclusionQuality = 0;
			}
		}
	}
}

bool UExGameplayLibrary::IsActorHidden(AActor* Actor)
{
	if (Actor)
	{
		return Actor->IsHidden();
	}
	return false;
}

bool UExGameplayLibrary::IsA(UObject* Object, UClass* Class)
{
	if (Object == nullptr)
	{
		return false;
	}
	return Object->IsA(Class);
}

UObject* UExGameplayLibrary::StaticLoadObject(UClass* Class, UObject* InOuter, const FString& Name)
{
	return ::StaticLoadObject(Class, InOuter, *Name);
}
