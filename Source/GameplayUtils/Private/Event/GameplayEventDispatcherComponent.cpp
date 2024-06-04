#include "Event/GameplayEventDispatcherComponent.h"
#include "GameplayUtilsModule.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"

void FGameplayWaitEventItem::Initialize(UObject* SourceObject)
{
	if (IsValid())
	{
		Watcher->SetSourceObject(SourceObject);
		Handler->SetSourceObject(SourceObject);
		Watcher->SuccessDelegate.AddDynamic(Handler, &UGameplayEventHandler::NativeExecute);
		Watcher->Activate();
	}
}

bool FGameplayWaitEventItem::IsValid() const
{
	return Watcher && Handler;
}

void UGameplayEventDispatcherComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);

	//加载配置
	for (auto ClassePtr : HandlerClasses)
	{
		if (!ClassePtr.IsNull())
		{
			if (TSubclassOf<UGameplayEventHandler> HandlerClass = ClassePtr.LoadSynchronous())
			{
				if (UGameplayEventHandler* Handler = NewObject<UGameplayEventHandler>(this, HandlerClass))
				{
					Handler->SetSourceObject(this);
					HandlerList.Add(Handler);
				}
			}
		}
	}

	//实例化的配置
	for (auto Handler : InstancedHandlerList)
	{
		if (Handler)
		{
			HandlerList.Add(Handler);
		}
	}

	for (auto& Event : WaitEventList)
	{
		Event.Initialize(this);
	}
}

void UGameplayEventDispatcherComponent::MulticastPlayAnimation_Implementation(USkeletalMeshComponent* SkeletalMeshComponent, UAnimationAsset* AnimationAsset, bool bLooping)
{
	if (!SkeletalMeshComponent || !AnimationAsset)
	{
		return;
	}

	SkeletalMeshComponent->PlayAnimation(AnimationAsset, bLooping);
}


void UGameplayEventDispatcherComponent::MulticastExecuteHandler_Implementation(TSubclassOf<UGameplayEventHandler> HandlerClass, const FGameplayEventContext& Context)
{
	ExecuteHandler(HandlerClass, Context);
}

void UGameplayEventDispatcherComponent::ExecuteHandler(TSubclassOf<UGameplayEventHandler> HandlerClass, const FGameplayEventContext& Context)
{
	for (auto& Handler : HandlerList)
	{
		if (Handler && Handler->GetClass() == HandlerClass)
		{
			Handler->NativeExecute(Context);
			return;
		}
	}
}

void UGameplayEventDispatcherComponent::ExecuteHandler(const FGameplayTag& HandlerTag, const FGameplayEventContext& Context)
{
	for (auto& Handler : HandlerList)
	{
		if (Handler && Handler->HandlerTags.HasTag(HandlerTag))
		{
			Handler->NativeExecute(Context);
		}
	}
}

void UGameplayEventDispatcherComponent::ExecuteHandlerWithTagAndInstigator(FGameplayTag HandlerTag, UObject* Instigator)
{
	FGameplayEventContext Context(Instigator);
	Context.EventTag = HandlerTag;
	ExecuteHandler(HandlerTag, Context);
}
