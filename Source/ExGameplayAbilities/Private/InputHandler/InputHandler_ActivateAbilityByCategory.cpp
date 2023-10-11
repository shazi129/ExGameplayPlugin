#include "InputHandler/InputHandler_ActivateAbilityByCategory.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ExGameplayAbilitiesModule.h"
#include "ExAbilitySystemComponent.h"

void UInputHandler_ActivateAbilityByCategory::NativeExecute(const FInputActionValue& inputValue)
{
	if (!CategoryTag.IsValid() || !SourceObject)
	{
		EXABILITY_LOG(Error, TEXT("%s error, CategoryTag: %s, SourceObject:%s"), *FString(__FUNCTION__), *CategoryTag.ToString(), *GetNameSafe(SourceObject));
		return;
	}

	AActor* Owner = Cast<AActor>(SourceObject);
	if (!Owner)
	{
		if (UActorComponent* ActorComponent = Cast<UActorComponent>(SourceObject))
		{
			Owner = ActorComponent->GetOwner();
		}
	}
	if (!Owner)
	{
		EXABILITY_LOG(Error, TEXT("%s error, SourceObject %s must be a actor or component"), *FString(__FUNCTION__), *GetNameSafe(SourceObject));
		return;
	}

	UExAbilitySystemComponent* ASC = Cast<UExAbilitySystemComponent>(Owner->GetComponentByClass(UExAbilitySystemComponent::StaticClass()));
	if (!ASC)
	{
		EXABILITY_LOG(Error, TEXT("%s error, Owner %s has no UExAbilitySystemComponent"), *FString(__FUNCTION__), *GetNameSafe(Owner));
		return;
	}

	ASC->ActivateAbilityByCategory(CategoryTag);
}
