#include "Subsystems/GameplayInstanceSubsystem.h"
#include "Macros/SubsystemMacros.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"

UGameplayInstanceSubsystem* UGameplayInstanceSubsystem::Get(const UObject* WorldContextObject)
{
	GET_GAMEINSTANCE_SUBSYSTEM(LogGameplayUtils, UGameplayInstanceSubsystem, WorldContextObject);
}

bool UGameplayInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	bool bShould = Super::ShouldCreateSubsystem(Outer);
	GAMEPLAYUTILS_LOG(Log, TEXT("UGameplayInstanceSubsystem::ShouldCreateSubsystem, %d, %s"), bShould, *GetNameSafe(Outer));
	return bShould;
}

void UGameplayInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UGameplayInstanceSubsystem::Initialize"));
	Super::Initialize(Collection);

	//world的创建和销毁
	WorldTearDownHandler = FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UGameplayInstanceSubsystem::OnWorldTearingDown);
	WorldInitializedActorsHandler = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UGameplayInstanceSubsystem::OnWorldInitializedActors);

	//apply生命周期
	FCoreDelegates::ApplicationWillDeactivateDelegate.AddLambda([]() {
		UE_LOG(LogTemp, Warning, TEXT("===Application will deactivate"));
	});

	FCoreDelegates::ApplicationHasReactivatedDelegate.AddLambda([]() {
		UE_LOG(LogTemp, Warning, TEXT("===Application has reactivated"));
	});

	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddLambda([]() {
		UE_LOG(LogTemp, Warning, TEXT("===Application will enter background"));
	});

	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddLambda([]() {
		UE_LOG(LogTemp, Warning, TEXT("===Application has entered foreground"));
	});

	FCoreDelegates::ApplicationWillTerminateDelegate.AddLambda([]() {
		UE_LOG(LogTemp, Warning, TEXT("===Application will terminate"));
	});

}

void UGameplayInstanceSubsystem::Deinitialize()
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UGameplayInstanceSubsystem::Deinitialize"));
	Super::Deinitialize();
	FWorldDelegates::OnWorldBeginTearDown.Remove(WorldTearDownHandler);
	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitializedActorsHandler);
}


void UGameplayInstanceSubsystem::OnWorldTearingDown(UWorld* World)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UGameplayInstanceSubsystem::OnWorldTearingDown %s"), *GetNameSafe(World));
	if (WorldTeardownDeletage.IsBound())
	{
		WorldTeardownDeletage.Broadcast(World);
	}
}

void UGameplayInstanceSubsystem::OnWorldInitializedActors(const UWorld::FActorsInitializedParams& Params)
{
	GAMEPLAYUTILS_LOG(Log, TEXT("UGameplayInstanceSubsystem::OnWorldInitializedActors %s"), *GetNameSafe(Params.World));
	if (WorldInitializedActorsDeletage.IsBound())
	{
		WorldInitializedActorsDeletage.Broadcast(Params.World);
	}
}
