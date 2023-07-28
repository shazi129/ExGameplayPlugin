#include "AsyncActions/AsyncAction_LoadPackage.h"
#include "GameplayUtilsModule.h"

UAsyncAction_LoadPackage* UAsyncAction_LoadPackage::AsyncLoadPackage(const FString& PackageName)
{
	UAsyncAction_LoadPackage* AsyncAction = NewObject<UAsyncAction_LoadPackage>();
	AsyncAction->PackageName = PackageName;
	return AsyncAction;
}

EBPAsyncLoadingResult UAsyncAction_LoadPackage::ConvertEAsyncLoadingResult(EAsyncLoadingResult::Type Result)
{
	switch (Result)
	{
	case EAsyncLoadingResult::Failed:
		return EBPAsyncLoadingResult::Failed;
		break;
	case EAsyncLoadingResult::Succeeded:
		return EBPAsyncLoadingResult::Succeeded;
		break;
	case EAsyncLoadingResult::Canceled:
		return EBPAsyncLoadingResult::Canceled;
		break;
	default:
		return EBPAsyncLoadingResult::Succeeded;
		break;
	}
}

void UAsyncAction_LoadPackage::UnLoadPackage(UPackage* Package)
{
	if (Package == nullptr)
	{
		GAMEPLAYUTILS_LOG(Error, TEXT("%s error, Package is null"), *FString(__FUNCTION__));
		return;
	}

	ForEachObjectWithPackage(Package, [](UObject* Object)
	{
		Object->ClearFlags(RF_Standalone);
		return true;
	}, false);
}

void UAsyncAction_LoadPackage::Activate()
{
	LoadPackageAsync(PackageName, FLoadPackageAsyncDelegate::CreateUObject(this, &UAsyncAction_LoadPackage::OnPackageLoadFinish));
}

void UAsyncAction_LoadPackage::OnPackageLoadFinish(const FName& InPackageName, UPackage* InPackage, EAsyncLoadingResult::Type InResult)
{
	if (OnFinish.IsBound())
	{
		FAsyncLoadPackageResult Result;
		Result.PackageName = InPackageName;
		Result.Package = InPackage;
		Result.Result = UAsyncAction_LoadPackage::ConvertEAsyncLoadingResult(InResult);

		OnFinish.Broadcast(Result);
	}
}