#pragma once

#include "CoreMinimal.h"
#include "DataVersionDescriptor.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYPLUGIN_API FDataVersionDescriptor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int64 Version;

	UPROPERTY(BlueprintReadOnly)
	FString Hash;

	UPROPERTY(BlueprintReadOnly)
	int ZoneID;

	UPROPERTY(BlueprintReadOnly)
	FString DataPath;

	UPROPERTY(BlueprintReadOnly)
	FString Environment;

	UPROPERTY(BlueprintReadOnly)
	FString Url;

	FDataVersionDescriptor();

	bool LoadFromFile(const FString& FilePath, FText& OutFailReason);
	bool LoadFromJson(const FString& JsonContent, FText& OutFailReason);

	bool IsValid() const;
	bool operator>(const FDataVersionDescriptor& Other) const;
	bool operator==(const FDataVersionDescriptor& Other) const;

	FString ToString() const;
};