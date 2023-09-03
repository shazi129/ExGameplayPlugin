#include "ExAbilityProvider.h"

void FAbilityProviderInfo::Reset()
{
	ProviderType = 0;
	ProviderID = 0;
	ProviderObject = nullptr;
}

bool FAbilityProviderInfo::IsValid() const
{
	return ProviderType > 0 || ProviderID > 0 || ProviderObject != nullptr;
}

bool FAbilityProviderInfo::operator==(const FAbilityProviderInfo& Other) const
{
	return ProviderType == Other.ProviderType && ProviderID == Other.ProviderID && ProviderObject == Other.ProviderObject;
}

FString FAbilityProviderInfo::ToString() const
{
	return FString::Printf(TEXT("%d, %d, %s"), ProviderType, ProviderID, ProviderObject);
}

FAbilityProviderInfo::FAbilityProviderInfo()
{
	Reset();
}

FAbilityProviderInfo::FAbilityProviderInfo(UObject* InProviderObject)
{
	Reset();
	ProviderObject = InProviderObject;
}

FAbilityProviderInfo::FAbilityProviderInfo(int InProviderType, int InProviderID, UObject* InProviderObject)
{
	ProviderType = InProviderType;
	ProviderID = InProviderID;
	ProviderObject = InProviderObject;
}

void FCollectedAbilityInfo::Reset()
{
	ProviderInfo.Reset();
	AbilityCases.Reset();
}
bool FCollectedAbilityInfo::IsVaild() const
{
	return ProviderInfo.IsValid();
}

FAbilityProviderInfo IExAbilityProvider::GetProviderInfo()
{
	return FAbilityProviderInfo(Cast<UObject>(this));
}
