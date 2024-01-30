#include "FunctionLibraries/PlatformHelperLibrary.h"

FString UPlatformHelperLibrary::GetComputerName()
{
	auto ComputerName = FPlatformProcess::ComputerName();
	return FString(ComputerName);
}

FString UPlatformHelperLibrary::GetPrimaryGPUBrand()
{
#if PLATFORM_WINDOWS
	return FWindowsPlatformMisc::GetPrimaryGPUBrand();
#else
	return FGenericPlatformMisc::GetPrimaryGPUBrand();
#endif
}

FString UPlatformHelperLibrary::GetCPUBrand()
{
#if PLATFORM_WINDOWS
	return FWindowsPlatformMisc::GetCPUBrand();
#else
	return FGenericPlatformMisc::GetCPUBrand();
#endif
}

void UPlatformHelperLibrary::ClipboardCopy(const FString& Value, bool LogValue)
{
	if (LogValue)
	{
		UE_LOG(LogTemp, Log, TEXT("%s: %s"), *FString(__FUNCTION__), *Value);
	}
#if PLATFORM_WINDOWS
	return FWindowsPlatformMisc::ClipboardCopy(*Value);
#else
	return FGenericPlatformMisc::ClipboardCopy(*Value);
#endif
}

EPlatformType UPlatformHelperLibrary::GetPlatformType()
{
#if PLATFORM_WINDOWS
	return EPlatformType::E_Windows;
#elif PLATFORM_IOS
	return EPlatformType::E_IOS;
#elif PLATFORM_ANDROID
	return EPlatformType::E_Android;
#else
	return EPlatformType::E_Unknow;
#endif
}


FString UPlatformHelperLibrary::GetIOSApplicationID()
{
#if PLATFORM_IOS
	NSString* bundleIdentifier = [[NSBundle mainBundle]bundleIdentifier];
	return FString(bundleIdentifier.UTF8String);
#else
	return FString();
#endif
}
