#include "GameplayObject.h"
#include "GameplayUtilsModule.h"

UWorld* UGameplayObject::GetWorld() const
{
	if (SourceObject != nullptr)
	{
		UWorld* World = SourceObject->GetWorld();
		return World;
	}

	return nullptr;
}

AActor* UGameplayObject::GetSourceActor()
{
	if (UActorComponent* Component = Cast<UActorComponent>(SourceObject))
	{
		return Component->GetOwner();
	}
	else if (AActor* Actor = Cast<AActor>(SourceObject))
	{
		return Actor;
	}
	return nullptr;
}

void UGameplayObject::SetSourceObject(UObject* Object)
{
	if (SourceObject != Object)
	{
		UObject* OldObject = SourceObject.Get();
		SourceObject = Object;

		NativeOnSourceObjectChange(OldObject, Object);
	}
	
}

void UGameplayObject::NativeOnSourceObjectChange(UObject* OldSourceObject, UObject* NewSourceObject)
{
	OnSourceObjectChange(OldSourceObject, NewSourceObject);
}
