
#include "ExGameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueSet.h"
#include "GameplayTagsManager.h"
#include "AssetRegistryModule.h"

UExGameplayCueManager::UExGameplayCueManager(class FObjectInitializer const& Initializer)
	:Super(Initializer)
{

}

UExGameplayCueManager* UExGameplayCueManager::GetGlobalsMOCueManager()
{
	return Cast<UExGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

#if WITH_EDITOR
void UExGameplayCueManager::FindCueAssets(FGameplayTag CueTag, TArray<FSoftObjectPath>& ResultToAppend)
{
	UGameplayCueSet* EditorSet = GetEditorCueSet();
	if (EditorSet == nullptr)
	{
		EditorSet = GetRuntimeCueSet();
	}
	if (!ensure(EditorSet))
	{
		return;
	}

	if (int32* idxPtr = EditorSet->GameplayCueDataMap.Find(CueTag))
	{
		int32 idx = *idxPtr;
		if (EditorSet->GameplayCueData.IsValidIndex(idx))
		{
			FGameplayCueNotifyData& Data = EditorSet->GameplayCueData[idx];
			ResultToAppend.Add(Data.GameplayCueNotifyObj);
		}
	}
}

void UExGameplayCueManager::FindDepentCueAssets(UObject* Object, TArray<FSoftObjectPath>& ResultToAppend)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetIdentifier> ReferenceNames;
	const UPackage* OuterPackage = Object->GetOutermost();
	AssetRegistryModule.Get().GetDependencies(FAssetIdentifier(OuterPackage->GetFName()), ReferenceNames);
	for (FAssetIdentifier& ReferenceName : ReferenceNames)
	{
		FGameplayTag CueTag = UGameplayTagsManager::Get().RequestGameplayTag(ReferenceName.ValueName,false);
		if (CueTag.IsValid())
		{
			FindCueAssets(CueTag, ResultToAppend);
		}
	}
}
#endif