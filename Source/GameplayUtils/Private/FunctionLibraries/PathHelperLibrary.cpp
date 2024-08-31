#include "FunctionLibraries/PathHelperLibrary.h"

FString UPathHelperLibrary::GetPackageFullName(const UObject* Object)
{
	if (Object == nullptr)
	{
		return "";
	}

	if (UPackage* Package = Object->GetPackage())
	{
#if UE_EDITOR
		if (Object->IsA(UWorld::StaticClass()) || Object->IsA(ULevel::StaticClass()))
		{
			if (UWorld* World = Object->GetWorld())
			{
				FString PackageFullName = Package->GetName();
				FString PackagePath = FPackageName::GetLongPackagePath(PackageFullName);
				FString PackageShortName = FPackageName::GetShortName(PackageFullName);
				PackageShortName.RemoveFromStart(World->StreamingLevelsPrefix);
				return FString::Printf(TEXT("%s/%s"), *PackagePath, *PackageShortName);
			}

		}
		return Package->GetName();
#else
		return Package->GetName();
#endif
	}
	return "";
}

FString UPathHelperLibrary::GetExternalAbsolutePath(const FString& RelativePath)
{
	IFileManager& FileManager = IFileManager::Get();
	return FileManager.ConvertToAbsolutePathForExternalAppForWrite(*RelativePath);
}

FString UPathHelperLibrary::ConvertPackagePathToClassPath(const FString& PackageFullPath)
{
	FString PackageShortName = FPackageName::GetShortName(PackageFullPath);
	FString ClassPath = FString::Printf(TEXT("%s.%s_C"), *PackageFullPath, *PackageShortName);
	return MoveTemp(ClassPath);
}

FString UPathHelperLibrary::GetPathName(UObject* Object)
{
	if (Object)
	{
		return Object->GetPathName();
	}
	return FString();
}

FString UPathHelperLibrary::GetSoftPtrPathName(TSoftObjectPtr<UObject> ObjectPtr)
{
	return ObjectPtr.GetLongPackageName();
}
