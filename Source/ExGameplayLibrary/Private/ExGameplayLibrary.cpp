#include "ExGameplayLibrary.h"
#include "ExGameplayLibraryModule.h"
#include "Misc/DateTime.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	return FDateTime::UtcNow().ToUnixTimestamp();
}

int UExGameplayLibrary::GetTimeZone()
{
	//本地时间与Utc时间差的小时数
	return (FDateTime::Now().ToUnixTimestamp() - FDateTime::UtcNow().ToUnixTimestamp()) / 3600;
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

	UE_LOG(LogExGameplayLibrary, Log, TEXT("UExGameplayLibrary::ExecCommand[%s]"), *Command);

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

bool UExGameplayLibrary::IsGameplayTagValid(FGameplayTag GameplayTag)
{
	return GameplayTag.IsValid();
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
bool UExGameplayLibrary::PawnInputEanbled(APawn* Pawn)
{
	return Pawn ? Pawn->InputEnabled() : false;
}

FString UExGameplayLibrary::GetComputerName()
{
	auto ComputerName = FPlatformProcess::ComputerName();
	return FString(ComputerName);
}

bool UExGameplayLibrary::IsRunning(UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			return World->HasBegunPlay();
		}
	}
	return false;
}

FString UExGameplayLibrary::GetPackageFullName(UObject* Object)
{
	if (Object == nullptr)
	{
		return "";
	}

	if (UPackage* Package = Object->GetPackage())
	{
#if UE_EDITOR
		if (Object->IsA(UWorld::StaticClass()) || Object->IsA(ULevel::StaticClass()))
		{
			if (UWorld* World = Object->GetWorld())
			{
				FString PackageFullName = Package->GetName();
				FString PackagePath = FPackageName::GetLongPackagePath(PackageFullName);
				FString PackageShortName = FPackageName::GetShortName(PackageFullName);
				PackageShortName.RemoveFromStart(World->StreamingLevelsPrefix);
				return FString::Printf(TEXT("%s/%s"), *PackagePath, *PackageShortName);
			}
			
		}
		return Package->GetName();
#else
		return Package->GetName();
#endif
	}
	return "";
}

FString UExGameplayLibrary::ObjectPathToPackageName(const FString& ObjectPath)
{
	return FPackageName::ObjectPathToPackageName(ObjectPath);
}

AActor* UExGameplayLibrary::SpawnActorInSocket(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, UPrimitiveComponent* Parent, FName Socket)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	AActor* Actor = World->SpawnActor(ActorClass);
	if (Actor)
	{
		Actor->AttachToComponent(Parent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), Socket);
	}
	else
	{
		UE_LOG(LogExGameplayLibrary, Error, TEXT("%s error, cannot spawn actor by class %s"), *FString(__FUNCTION__), *GetNameSafe(ActorClass));
	}
	return Actor;
}


bool UExGameplayLibrary::ActorLineTraceSingle(AActor* Actor, FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel)
{
	if (Actor)
	{
		FCollisionQueryParams Params;
		Params.TraceTag = FName("ActorLineTraceSingle");
		Params.bTraceComplex = false;
		return Actor->ActorLineTraceSingle(OutHit, Start, End, TraceChannel, Params);
	}
	return false;
}

void UExGameplayLibrary::CharacterSmoothMoveTo(ACharacter* Character, FVector TargetLocation, float DeltaSecond)
{
	if (Character)
	{
		if (DeltaSecond == 0.0f)
		{
			DeltaSecond = 0.03f;
		}
		FVector Velocity = (TargetLocation - Character->GetActorLocation()) / DeltaSecond;
		Character->GetCharacterMovement()->MoveSmooth(Velocity, DeltaSecond);
	}
}

bool UExGameplayLibrary::IsValidObject(UObject* Object)
{
	return Object != nullptr && !Object->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed);
}

bool UExGameplayLibrary::ParseParamIntValue(const FString& InString, const FString& InParam, int& OutValue)
{
	FString ParamStringValue;
	bool Result = UKismetSystemLibrary::ParseParamValue(InString, InParam, ParamStringValue);
	if (Result && !ParamStringValue.IsEmpty())
	{
		OutValue = FCString::Atoi(*ParamStringValue);
		return true;
	}

	return false;
}
