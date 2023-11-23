#include "InputHandler/InputHandler_ActivateAbilityByTag.h"
#include "ExGameplayAbilitiesModule.h"

UExAbilitySystemComponent* AbilityInpuHandlerHelper::GetAbilitySystem(UObject* SourceObject)
{
	if (SourceObject == nullptr)
	{
		EXABILITY_LOG(Error, TEXT("%s error, SourceObject is null"), *FString(__FUNCTION__));
		return nullptr;
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
		return nullptr;
	}

	UExAbilitySystemComponent* ASC = Cast<UExAbilitySystemComponent>(Owner->GetComponentByClass(UExAbilitySystemComponent::StaticClass()));
	if (!ASC)
	{
		EXABILITY_LOG(Error, TEXT("%s error, Owner %s has no UExAbilitySystemComponent"), *FString(__FUNCTION__), *GetNameSafe(Owner));
		return nullptr;
	}

	return ASC;
}

void AbilityInpuHandlerHelper::ActivateAbilityByCategory(UExAbilitySystemComponent* ASC, const FGameplayTag& CategoryTag, bool ToServer)
{
	if (!ASC || !CategoryTag.IsValid())
	{
		EXABILITY_LOG(Error, TEXT("%s error, ASC or CategoryTag is not valid"), *FString(__FUNCTION__));
		return;
	}
	if (ToServer)
	{
		ASC->ServerActivateAbilityByCategory(CategoryTag);
	}
	else
	{
		ASC->ActivateAbilityByCategory(CategoryTag);
	}
}


void UInputHandler_ActivateAbilityByTag::NativeExecute(const FInputActionValue& inputValue)
{
}

