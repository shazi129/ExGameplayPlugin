
#pragma once

#include "GameplayCueManager.h"
#include "ExGameplayCueManager.generated.h"

UCLASS()
class EXGAMEPLAYABILITIES_API UExGameplayCueManager :public UGameplayCueManager
{
	GENERATED_UCLASS_BODY()
public:
	static UExGameplayCueManager* GetGlobalsMOCueManager();
#if WITH_EDITOR
	//查找Tag对应的Cue对象
	void FindCueAssets(FGameplayTag CueTag,TArray<FSoftObjectPath>& ResultToAppend);
	void FindDepentCueAssets(UObject* Object, TArray<FSoftObjectPath>& ResultToAppend);
#endif
};
