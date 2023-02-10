#include "DataCenter/DataVersionDescriptor.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "DataVersionDescriptor"

FDataVersionDescriptor::FDataVersionDescriptor()
	: Version(0)
	, Hash("")
	, ZoneID(0)
	, DataPath("")
{

}

bool FDataVersionDescriptor::LoadFromFile(const FString& FilePath, FText& OutFailReason)
{
	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *FilePath))
	{
		OutFailReason = FText::FromString(FString::Printf(TEXT("Failed to open descriptor file %s"), *FilePath));
		return false;
	}

	if (!LoadFromJson(FileContents, OutFailReason))
	{
		return false;
	}
	DataPath = FPaths::GetPath(FilePath) + "/";

	return true;
}

bool FDataVersionDescriptor::LoadFromJson(const FString& JsonContent, FText& OutFailReason)
{
	TSharedPtr< FJsonObject > Object;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JsonContent);
	if (!FJsonSerializer::Deserialize(Reader, Object) || !Object.IsValid())
	{
		OutFailReason = FText::FromString(FString::Printf(TEXT("Cannot serialize content to json: %s"), *JsonContent));
		return false;
	}

	//必须字段
	Version = 0;
	if (!Object->TryGetNumberField(TEXT("Version"), Version) || Version <= 0)
	{
		OutFailReason = LOCTEXT("InvalidVersion", "File does not have a valid 'Version' number.");
		return false;
	}

	Hash = "";
	if (!Object->TryGetStringField(TEXT("MD5"), Hash) || Hash.IsEmpty())
	{
		OutFailReason = LOCTEXT("InvalidVersion", "File does not have a valid 'MD5' String.");
		return false;
	}

	Environment = "";
	if (!Object->TryGetStringField(TEXT("Environment"), Environment) || Environment.IsEmpty())
	{
		OutFailReason = LOCTEXT("InvalidVersion", "File does not have a valid 'Environment' string.");
		return false;
	}

	//可选字段
	ZoneID = -1;
	Object->TryGetNumberField(TEXT("ZoneID"), ZoneID);

	Url = "";
	Object->TryGetStringField(TEXT("Url"), Url);

	return true;
}

bool FDataVersionDescriptor::IsValid() const
{
	return Version > 0 && !Hash.IsEmpty();
}

bool FDataVersionDescriptor::operator>(const FDataVersionDescriptor& Other) const
{
	return Version > Other.Version && Hash != Other.Hash;
}

bool FDataVersionDescriptor::operator==(const FDataVersionDescriptor& Other) const
{
	return Hash == Other.Hash;
}

FString FDataVersionDescriptor::ToString() const
{
	return FString::Printf(TEXT("Version:%lld, Hash:%s, ZoneID:%d, Environment:%s"), Version, *Hash, ZoneID, *Environment);
}

#undef LOCTEXT_NAMESPACE