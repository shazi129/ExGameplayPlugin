#include "UMG/ExUserWidget.h"

void UExUserWidget::NativeOnInitialized()
{
	for (auto& ExtensionClassPtr : ExtensionClasses)
	{
		if (!ExtensionClassPtr.IsNull())
		{
			TSubclassOf<UUserWidgetExtension> ExtensionClass = ExtensionClassPtr.LoadSynchronous();
			if (ExtensionClass)
			{
				AddExtension(ExtensionClass);
			}
		}
	}
	Super::NativeOnInitialized();
}