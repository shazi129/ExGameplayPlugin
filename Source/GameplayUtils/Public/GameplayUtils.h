#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

namespace GameplayUtils
{
	template<class T>
	T* GetActorComponent(AActor* Actor)
	{
		static_assert(std::is_base_of<UActorComponent, T>::value, "T must be a subclass of UActorComponent");
		return Cast<T>(Actor->GetComponentByClass(T::StaticClass()));
	}

	template<class T>
	T* GetActorComponent(UActorComponent* Component)
	{
		static_assert(std::is_base_of<UActorComponent, T>::value, "T must be a subclass of UActorComponent");
		if (Component)
		{
			if (AActor* Owner = Component->GetOwner())
			{
				return Cast<T>(Owner->GetComponentByClass(T::StaticClass()));
			}
		}
		return nullptr;
	}

	bool FilterActorClasses(AActor* Actor, const TArray<TSubclassOf<AActor>>& ActorClasses);
}