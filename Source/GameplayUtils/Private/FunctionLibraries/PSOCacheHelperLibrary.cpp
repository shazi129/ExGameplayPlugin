#include "FunctionLibraries/PSOCacheHelperLibrary.h"

#include "ShaderPipelineCache.h"

bool UPSOCacheHelperLibrary::EnableShaderPipelineCache(bool bEnable)
{
	UE_LOG(LogTemp, Display, TEXT("EnableShaderPipelineCache %s"), bEnable ? TEXT("true") : TEXT("false"));
	auto Var = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ShaderPipelineCache.Enabled"));
	if (Var)
	{
		Var->Set(bEnable ? 1 : 0);
	}
	return !!Var;
}

bool UPSOCacheHelperLibrary::EnableLogPSO(bool bEnable)
{
	auto Var = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ShaderPipelineCache.LogPSO"));
	if (Var)
	{
		Var->Set(bEnable ? 1 : 0);
	}
	return !!Var;
}

// bool UPSOCacheHelperLibrary::LoadShaderPipelineCache()
// {
// 	UE_LOG(LogTemp, Display, TEXT("Load Shader pipeline cache %s for platform"), FApp::GetProjectName());
// 	return FShaderPipelineCache::OpenPipelineFileCache(GMaxRHIShaderPlatform);
// }

bool UPSOCacheHelperLibrary::SavePipelineFileCache()
{
	return FShaderPipelineCache::SavePipelineFileCache(FPipelineFileCacheManager::SaveMode::BoundPSOsOnly);
}
