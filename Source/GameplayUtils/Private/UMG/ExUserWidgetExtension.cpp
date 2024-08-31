#include "UMG/ExUserWidgetExtension.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"

UExUserWidgetExtension::UExUserWidgetExtension()
{
}

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
	if (bTickable)
	{
		Super::Tick(MyGeometry, InDeltaTime);
		ReceiveTick(MyGeometry, InDeltaTime);
	}
}

UUserWidget* UExUserWidgetExtension::GetOuterUserWidget()
{
	return GetUserWidget();
}

void UExUserWidgetExtension::SetVisibility(ESlateVisibility InVisibility)
{
	if (UUserWidget* UserWidget = GetUserWidget())
	{
		UserWidget->SetVisibility(InVisibility);
	}
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
		if (Extension != nullptr && ExtensionName.IsEqual(Extension->GetFName(), ENameCase::IgnoreCase, false))
		{
			return Extension;
		}
	}
	return nullptr;
}

UUserWidgetExtension* UExUserWidgetExtension::GetWidgetComponentExtensionByClass(UWidgetComponent* Widget, TSubclassOf<UUserWidgetExtension> Class)
{
	if (!Widget || !Class)
	{
		return nullptr;
	}

	UUserWidget* UserWidget = Widget->GetWidget();
	if (!UserWidget)
	{
		return nullptr;
	}
	return UserWidget->GetExtension(Class);
}

bool UExUserWidgetExtension::IsTickable_Implementation() const
{
	return bTickable;
}


