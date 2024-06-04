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
}