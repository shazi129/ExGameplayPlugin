#include "InputHandler/InputHandler_TriggerInputTag.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ExInputSystemModule.h"
#include "ExInputSubsystem.h"
#include "Components/PlayerControlsComponent.h"

void UInputHandler_TriggerInptutTag::NativeExecute(const FInputActionValue& inputValue)
{
	UExInputSubsystem::TriggerInputTag(this, InputTag);

	//如果需要复制到服务端
	if (ToServer)
	{
		ACharacter* LocalCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (LocalCharacter && LocalCharacter->GetLocalRole() == ROLE_AutonomousProxy) //当前必须是主控端
		{
			UPlayerControlsComponent* ControlComp = Cast<UPlayerControlsComponent>(LocalCharacter->GetComponentByClass(UPlayerControlsComponent::StaticClass()));
			if (ControlComp)
			{
				ControlComp->ServerTriggerInputTag(InputTag);
			}
		}
	}
}