#include "FunctionLibraries/TimeHelperLibrary.h"
#include "Misc/DateTime.h"

int64 UTimeHelperLibrary::GetTimestamp()
{
	return FDateTime::UtcNow().ToUnixTimestamp();
}

int64 UTimeHelperLibrary::GetTimestampMs()
{
	return (FDateTime::UtcNow() - FDateTime(1970, 1, 1)).GetTotalMilliseconds();
}

int UTimeHelperLibrary::GetTimeZone()
{
	//本地时间与Utc时间差的小时数
	return (FDateTime::Now().ToUnixTimestamp() - FDateTime::UtcNow().ToUnixTimestamp()) / 3600;
}

double UTimeHelperLibrary::GetCurrentFrameTime()
{
	return FApp::GetCurrentTime();
}

FString UTimeHelperLibrary::FormatNowString(const FString& Format)
{
	return FDateTime::Now().ToString(*Format);
}

FString UTimeHelperLibrary::FormatTimestampString(int64 TimestampSec, const FString& Format)
{
	FDateTime DateTime = FDateTime::FromUnixTimestamp(TimestampSec);
	return DateTime.ToString(*Format);
}
