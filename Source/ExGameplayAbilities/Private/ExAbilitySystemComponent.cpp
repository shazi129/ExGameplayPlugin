#include "ExAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "ExGameplayAbilitiesModule.h"
#include "ExGameplayLibrary.h"
#include "ExAbilityProvider.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"

void UExAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (AbilityActorInfo)
	{
		if (AbilityActorInfo->AnimInstance == nullptr)
		{
			AbilityActorInfo->AnimInstance = AbilityActorInfo->GetAnimInstance();
		}

		if (UGameInstance* GameInstance = InOwnerActor->GetGameInstance())
		{
			// Sign up for possess/unpossess events so that we can update the cached AbilityActorInfo accordingly
			//GameInstance->GetOnPawnControllerChanged().AddDynamic(this, &UExAbilitySystemComponent::OnPawnControllerChanged);
		}
	}

	//GrantDefaultAbilitiesAndAttributes();
}

void UExAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	//�ռ�Owner���ϵ�Ability
	const TSet<UActorComponent*>& OwnedComponents = GetOwner()->GetComponents();
	for (UActorComponent* Component : OwnedComponents)
	{
		IExAbilityProvider* AbilityProvider = Cast<IExAbilityProvider>(Component);
		if (AbilityProvider)
		{
			RegisterAbilityProvider(AbilityProvider);
		}
	}
}

void UExAbilitySystemComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UExAbilitySystemComponent::BeginDestroy()
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor.IsValid())
	{
		if (UGameInstance* GameInstance = AbilityActorInfo->OwnerActor->GetGameInstance())
		{
			GameInstance->GetOnPawnControllerChanged().RemoveAll(this);
		}
	}
	Super::BeginDestroy();
}

void UExAbilitySystemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UExAbilitySystemComponent::CollectAbilitCases(TArray<FExAbilityCase>& Abilities) const
{
	for (const FExAbilityCase& Ability : DefaultAbilities)
	{
		Abilities.Add(Ability);
	}
}

void UExAbilitySystemComponent::RegisterAbilityProvider(IExAbilityProvider* ProviderObject)
{
	if (ProviderObject == nullptr)
	{
		return;
	}

	if (GetOwner()->HasAuthority() == false)
	{
		EXABILITY_LOG(Log, TEXT("UExAbilitySystemComponent::RegisterAbilityProvider ignore, has no authority"));
		return;
	}

	TArray<FExAbilityCase> AbilityCases;
	ProviderObject->CollectAbilitCases(AbilityCases);
	for (const FExAbilityCase& AbilityCase : AbilityCases)
	{
		GiveAbilityByCaseInternal(AbilityCase, Cast<UObject>(ProviderObject));
	}
}

void UExAbilitySystemComponent::UnregisterAbilityProvider(IExAbilityProvider* ProviderObject)
{

}

FGameplayAbilitySpec* UExAbilitySystemComponent::FindAbilitySpecFromCase(const FExAbilityCase& AbilityCase)
{
	if (AbilityCase.IsValid() == false)
	{
		return nullptr;
	}

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability == AbilityCase.AbilityClass->GetDefaultObject())
		{
			return &Spec;
		}
	}

	return nullptr;
}

void UExAbilitySystemComponent::OnPawnControllerChanged(APawn* Pawn, AController* NewController)
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor == Pawn && AbilityActorInfo->PlayerController != NewController)
	{
		// Reinit the cached ability actor info (specifically the player controller)
		AbilityActorInfo->InitFromActor(AbilityActorInfo->OwnerActor.Get(), AbilityActorInfo->AvatarActor.Get(), this);
	}
}

FGameplayAbilitySpecHandle UExAbilitySystemComponent::GiveAbilityByCase(const FExAbilityCase& AbilityCase, UObject* AbilityProvider)
{
	return GiveAbilityByCaseInternal(AbilityCase, AbilityProvider);
}

FGameplayAbilitySpecHandle UExAbilitySystemComponent::GiveAbilityByCaseInternal(const FExAbilityCase& AbilityCase, UObject* AbilityProvider)
{
	if (!AbilityProvider)
	{
		AbilityProvider = this;
	}

	if (AbilityCase.IsValid() == false)
	{
		ABILITY_LOG(Error, TEXT("UExAbilitySystemComponent::GiveAbilityByCaseInternal error, AbilityCase Invalid"));
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpec Spec(AbilityCase.AbilityClass, AbilityCase.AbilityLevel, INDEX_NONE, (AbilityProvider));
	const FGameplayAbilitySpecHandle& SpecHandle = GiveAbility(Spec);

	if (AbilityCase.ActivateWhenGiven)
	{
		TryActivateAbility(SpecHandle);
	}

	return SpecHandle;
}

void UExAbilitySystemComponent::TryActivateAbilityOnceWithEventData_Implementation(const FExAbilityCase& AbilityCase, const FGameplayEventData& TriggerEventData, UObject* SourceObj)
{
	//ͨ��class �ҵ�handler
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromCase(AbilityCase);
	if (Spec && Spec->Handle.IsValid())
	{
		InternalTryActivateAbility(Spec->Handle, FPredictionKey(), nullptr, nullptr, &TriggerEventData);
	}
	else
	{
		EXABILITY_LOG(Error, TEXT("UExAbilitySystemComponent::TryActivateAbilityOnceWithEventData error, Spec invalid"));
	}
}

bool UExAbilitySystemComponent::TryActivateAbilityOnceWithEventData_Validate(const FExAbilityCase& AbilityCase, const FGameplayEventData& TriggerEventData, UObject* SourceObj)
{
	return true;
}

void UExAbilitySystemComponent::TryActivateAbilityByCase(const FExAbilityCase& AbilityCase)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromCase(AbilityCase);
	if (Spec && Spec->Handle.IsValid())
	{
		TryActivateAbility(Spec->Handle);
	}
	else
	{
		EXABILITY_LOG(Error, TEXT("UExAbilitySystemComponent::TryActivateAbilityByCase error, Handle for case is invalid"));
		return;
	}

	
}

void UExAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
}

void UExAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);
}

void UExAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
}

void UExAbilitySystemComponent::ClientPlayMontage_Implementation(
		UGameplayAbility* AnimatingAbility,
		FGameplayAbilityActivationInfo ActivationInfo,
		UAnimMontage* Montage, float InPlayRate,
		FName StartSectionName, float StartTimeSeconds)
{
	//只在主控端播放
	ENetRole OwnerRole = GetOwner()->GetLocalRole();
	if (OwnerRole == ENetRole::ROLE_AutonomousProxy)
	{
		PlayMontage(AnimatingAbility, ActivationInfo, Montage, InPlayRate, StartSectionName, StartTimeSeconds);
	}
}


void  UExAbilitySystemComponent::ClientStopMontage_Implementation()
{
	ENetRole OwnerRole = GetOwner()->GetLocalRole();
	if (OwnerRole == ENetRole::ROLE_AutonomousProxy)
	{
		CurrentMontageStop();
	}
}
