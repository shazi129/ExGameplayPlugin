#pragma once

#include "CoreMinimal.h"

struct GAMEPLAYUTILS_API FDebugLineContent
{
	FString Content;

	UFont* Font;
	FColor Color;

	bool bDisplay;
	
	void Reset()
	{
		Content.Reset();
		Font = GEngine->GetSmallFont();
		bDisplay = false;
	}

	FDebugLineContent()
	{
		Reset();
	}

	void Set(const FString& InContent, const FColor& InColor, UFont* InFont = nullptr);
};


struct GAMEPLAYUTILS_API FDebugDisplayProxy
{
	FDebugDisplayProxy();
	void DisplayDebug(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);
	void AddDebugLine(const FString& Content, const FColor& Color, UFont* Font=nullptr);
	void ClearContent();
	TWeakObjectPtr<UObject> DebugObject;

private:
	TArray<FDebugLineContent> Contents;
	
};