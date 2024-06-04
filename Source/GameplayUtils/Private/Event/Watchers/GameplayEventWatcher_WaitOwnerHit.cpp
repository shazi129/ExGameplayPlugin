#include "Event/Watchers/GameplayEventWatcher_WaitOwnerHit.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"
#include "GameplayUtilsModule.h"

FInstancedStruct FHitResultContext::MakeContext(UPrimitiveComponent* InHitComp, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, FVector InNormalImpulse, const FHitResult& InHit)
{
	FInstancedStruct Context;
	Context.InitializeAs<FHitResultContext>();
	FHitResultContext& HitResultContext = Context.GetMutable<FHitResultContext>();

	HitResultContext.HitComp = InHitComp;
	HitResultContext.OtherActor = InOtherActor;
	HitResultContext.OtherComp = InOtherComp;
	HitResultContext.NormalImpulse = InNormalImpulse;
	HitResultContext.HitBoneName = InHit.BoneName;

	return MoveTemp(Context);
}

void UGameplayEventWatcher_WaitOwnerHit::Activate()
{
	AActor* SourceActor = GetSourceActor();
	if (!SourceActor)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, Cannot find source actor by %s"), *FString(__FUNCTION__), *GetNameSafe(SourceObject.Get()));
		return;
	}

	for (auto& ComponentName : ComponentNames)
	{
		auto Component = Cast<UPrimitiveComponent>(UGameplayUtilsLibrary::GetComponentByName(SourceActor, ComponentName));
		if (Component)
		{
			Component->OnComponentHit.AddDynamic(this, &UGameplayEventWatcher_WaitOwnerHit::OnHit);
			WaitComponents.Add(Component);
		}
	}
}

void UGameplayEventWatcher_WaitOwnerHit::Deactivate()
{
	for (auto& WaitComponent : WaitComponents)
	{
		if (WaitComponent.IsValid())
		{
			WaitComponent->OnComponentHit.RemoveDynamic(this, &UGameplayEventWatcher_WaitOwnerHit::OnHit);
		}
	}
	WaitComponents.Empty();
}

void UGameplayEventWatcher_WaitOwnerHit::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!UGameplayUtilsLibrary::FilterActorClasses(OtherActor, ActorClasses))
	{
		return;
	}

	if (SuccessDelegate.IsBound())
	{
		FGameplayEventContext Context;
		Context.Instigator = this;
		Context.Data = FHitResultContext::MakeContext(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
		SuccessDelegate.Broadcast(Context);
	}
}

FHitResultContext UGameplayEventWatcher_WaitOwnerHit::GetContext(const FInstancedStruct& ContextData)
{
	if (ContextData.GetScriptStruct() == FHitResultContext::StaticStruct())
	{
		return ContextData.GetMutable<FHitResultContext>();
	}
	return FHitResultContext();
}

