#include "Tasks/AbilityTask_RegisterProvider.h"

UAbilityTask_RegisterProvider* UAbilityTask_RegisterProvider::RegisterProvider(UGameplayAbility* OwningAbility, UExAbilitySystemComponent* ASC, TScriptInterface<IExAbilityProvider> Provider, int WaitFrames)
{
	UAbilityTask_RegisterProvider* Task = NewAbilityTask<UAbilityTask_RegisterProvider>(OwningAbility);
	Task->ASC = ASC;
	Task->Provider = Provider;
	Task->WaitFrames = WaitFrames;
	return Task;
}

void UAbilityTask_RegisterProvider::Activate()
{
	Super::Activate();
}

void UAbilityTask_RegisterProvider::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_RegisterProvider::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
}

bool UAbilityTask_RegisterProvider::DoTaskWork()
{
	if (ASC && Provider)
	{
		ASC->RegisterAbilityProvider(Provider);
		OnFinish.Broadcast();
	}
	else
	{
		OnError.Broadcast();
	}
	EndTask();
	return true;
}
