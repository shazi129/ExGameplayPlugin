//Blueprint Library for FFileHelper


#pragma once

#include "FileHelperLibrary.generated.h"

UCLASS()
class EXGAMEPLAYLIBRARY_API UFileHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FileHelperLibrary")
	static bool LoadFileToArray(TArray<uint8>& Result, const FString& FileName, int Flags);
};