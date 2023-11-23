#include "Debug/DebugDisplay.h"
#include "Engine/Canvas.h"


void FDebugLineContent::Set(const FString& InContent, const FColor& InColor, UFont* InFont)
{
	Content = InContent;
	Color = InColor;
	Font = InFont ? InFont : GEngine->GetSmallFont();
}

FDebugDisplayProxy::FDebugDisplayProxy()
{
	DebugObject = nullptr;
}

void FDebugDisplayProxy::DisplayDebug(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
	if (!Canvas)
	{
		return;
	}

	float CurrentYPos = YPos;

	for (const FDebugLineContent& DebugContent : Contents)
	{
		Canvas->SetDrawColor(DebugContent.Color);
		FFontRenderInfo RenderInfo = FFontRenderInfo();
		RenderInfo.bEnableShadow = true;

		//Y方向上的缩放
		float ScaleY = 1.0f;
		float ScaleX = 1.0f;

		Canvas->DrawText(DebugContent.Font, DebugContent.Content, 0.0f, CurrentYPos, ScaleX, ScaleY, RenderInfo);
		CurrentYPos += (DebugContent.Font->GetMaxCharHeight() * ScaleY);
	}

	YL = Contents.Num();
	YPos = CurrentYPos - YPos;
}

void FDebugDisplayProxy::AddDebugLine(const FString& Content, const FColor& Color, UFont* Font)
{
	Contents.Emplace();
	Contents.Last().Set(Content, Color, Font);
}

void FDebugDisplayProxy::ClearContent()
{
	Contents.Reset();
}

