// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExEditorToolsStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FExEditorToolsStyle::StyleInstance = nullptr;

void FExEditorToolsStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FExEditorToolsStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FExEditorToolsStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ExEditorToolsStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FExEditorToolsStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ExEditorToolsStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ExGameplayPlugin")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ExEditorTools.OpenPluginWindow", new IMAGE_BRUSH(TEXT("ToolIcon_40x"), Icon40x40));

	return Style;
}

void FExEditorToolsStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FExEditorToolsStyle::Get()
{
	return *StyleInstance;
}
