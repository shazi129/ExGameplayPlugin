#include "UMG/ExUserWidgetExtension.h"
#include "Blueprint/UserWidget.h"

void UExUserWidgetExtension::Initialize()
{
	Super::Initialize();
	ReceiveInitialize();
}

void UExUserWidgetExtension::Construct()
{
	Super::Construct();
	ReceiveConstruct();
}

void UExUserWidgetExtension::Destruct()
{
	Super::Destruct();
	ReceiveDestruct();
}

bool UExUserWidgetExtension::RequiresTick() const
{
	return IsTickable();
}

void UExUserWidgetExtension::Tick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::Tick(MyGeometry, InDeltaTime);
	ReceiveTick(MyGeometry, InDeltaTime);
}

UUserWidget* UExUserWidgetExtension::GetOuterUserWidget()
{
	return GetUserWidget();
}

UUserWidgetExtension* UExUserWidgetExtension::GetUserWidgetExtensionByName(UUserWidget* UserWidget, FName ExtensionName)
{
	if (!UserWidget)
	{
		return nullptr;
	}

	TArray<UUserWidgetExtension*> Extensions = UserWidget->GetExtensions(UUserWidgetExtension::StaticClass());
	for (UUserWidgetExtension* Extension : Extensions)
	{
		if (Extension != nullptr && Extension->GetFName() == ExtensionName)
		{
			return Extension;
		}
	}
	return nullptr;
}

bool UExUserWidgetExtension::IsTickable_Implementation() const
{
	return false;
}


