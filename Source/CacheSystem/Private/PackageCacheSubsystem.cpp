#include "PackageCacheSubsystem.h"

void UPackageCacheSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UPackageCacheSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

int32 UPackageCacheSubsystem::LoadPackageAsync(const FName& InName)
{
	return ::LoadPackageAsync(InName.ToString(), FLoadPackageAsyncDelegate::CreateUObject(this, &UPackageCacheSubsystem::OnPackageLoadComplete));
}

void UPackageCacheSubsystem::UnloadPackage(const FName& InName)
{
	if (CachedPackages.Contains(InName))
	{
		TArray<UPackage*> Packages;
		Packages.Add(CachedPackages[InName]);

		FText OutErrorMessage;
		//UPackageTools::UnloadPackages(Packages, OutErrorMessage);
	}
}

void UPackageCacheSubsystem::OnPackageLoadComplete(const FName& PackageName, UPackage* Package, EAsyncLoadingResult::Type Result)
{
	if (CachedPackages.Contains(PackageName))
	{
		CachedPackages[PackageName] = Package;
	}
	else
	{
		CachedPackages.Add(PackageName, Package);
	}

	if (OnAsyncLoadFinish.IsBound())
	{
		OnAsyncLoadFinish.Broadcast(PackageName, Package, Result);
	}
}

