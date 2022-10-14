#include "FileHelperLibrary.h"

bool UFileHelperLibrary::LoadFileToArray(TArray<uint8>& Result, const FString& FileName, int Flags)
{
	return FFileHelper::LoadFileToArray(Result, *FileName, Flags);
}