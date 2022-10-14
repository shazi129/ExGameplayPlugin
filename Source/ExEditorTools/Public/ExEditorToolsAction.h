#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ExEditorToolsAction.generated.h"

UCLASS(BlueprintType)
class EXEDITORTOOLS_API UExEditorActionDefine : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced)
	UExEditorAction* Action;
};

UCLASS(BlueprintType, EditInlineNew, Abstract)
class EXEDITORTOOLS_API UExEditorAction : public UObject
{
	GENERATED_BODY()
public:
	virtual void DoAction() const PURE_VIRTUAL(UExEditorAction::DoAction)
};

UCLASS(BlueprintType, EditInlineNew)
class EXEDITORTOOLS_API UExEditorAction_OpenWidget : public UExEditorAction
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowedClasses = "EditorUtilityWidgetBlueprint"))
	FSoftObjectPath WidgetBlueprintPath;

public:
	virtual void DoAction() const override;
};

UCLASS(BlueprintType, EditInlineNew)
class EXEDITORTOOLS_API UExEditorAction_RunBlueprint: public UExEditorAction
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowedClasses = "EditorUtilityBlueprint"))
		FSoftObjectPath UtilityBlueprintPath;

public:
	virtual void DoAction() const override;
};

UCLASS(BlueprintType, EditInlineNew)
class EXEDITORTOOLS_API UExEditorAction_RunPython : public UExEditorAction
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (MultiLine = "true"))
	FText Script;

public:
	virtual void DoAction() const override;
};
