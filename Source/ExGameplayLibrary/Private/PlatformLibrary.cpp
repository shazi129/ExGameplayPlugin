#include "PlatformLibrary.h"

FString UPlatformLibrary::GetComputerName()
{
	auto ComputerName = FPlatformProcess::ComputerName();
	return FString(ComputerName);
}

FString UPlatformLibrary::GetPrimaryGPUBrand()
{
#if PLATFORM_WINDOWS
	return FWindowsPlatformMisc::GetPrimaryGPUBrand();
#else
	return FGenericPlatformMisc::GetPrimaryGPUBrand();
#endif
}

FString UPlatformLibrary::GetCPUBrand()
{
#if PLATFORM_WINDOWS
	return FWindowsPlatformMisc::GetCPUBrand();
#else
	return FGenericPlatformMisc::GetCPUBrand();
#endif
}

FMemoryInfo UPlatformLibrary::GetMemoryInfo()
{
	FPlatformMemoryConstants data = FPlatformMemory::GetConstants();

	FMemoryInfo MemoryInfo;
	MemoryInfo.TotalPhysical = data.TotalPhysical;
	MemoryInfo.TotalVirtual = data.TotalVirtual;
	MemoryInfo.TotalPhysicalGB = data.TotalPhysicalGB;
	return MemoryInfo;
}

void UPlatformLibrary::ClipboardCopy(const FString& Value)
{
#if PLATFORM_WINDOWS
	return FWindowsPlatformMisc::ClipboardCopy(*Value);
#else
	return FGenericPlatformMisc::ClipboardCopy(*Value);
#endif
}