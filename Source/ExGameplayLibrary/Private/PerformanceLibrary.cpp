#include "PerformanceLibrary.h"
#include "ExGameplayLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameUserSettings.h"


void UPerformanceLibrary::SetScalability(FScalabilityInfo ScalabilityInfo)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ResolutionQuality %d"), ScalabilityInfo.ResolutionScale));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ViewDistanceQuality %d"), ScalabilityInfo.ViewDistance));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.AntiAliasingQuality %d"), ScalabilityInfo.AntiAliasing));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.PostProcessQuality %d"), ScalabilityInfo.PostProcess));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ShadowQuality %d"), ScalabilityInfo.Shadow));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.GlobalIlluminationQuality %d"), ScalabilityInfo.GlobalIllumination));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ReflectionQuality %d"), ScalabilityInfo.Reflection));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.TextureQuality %d"), ScalabilityInfo.Texture));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.EffectsQuality %d"), ScalabilityInfo.Effects));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.FoliageQuality %d"), ScalabilityInfo.Foliage));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ShadingQuality %d"), ScalabilityInfo.Shading));
}

void UPerformanceLibrary::SetResolutionQuality(int ResolutionScale)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ResolutionQuality %d"), ResolutionScale));
}

void UPerformanceLibrary::SetViewDistanceQuality(int ViewDistanceQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ViewDistanceQuality %d"), ViewDistanceQuality));
}

void UPerformanceLibrary::SetAntiAliasingQuality(int AntiAliasingQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.AntiAliasingQuality %d"), AntiAliasingQuality));
}

void UPerformanceLibrary::SetPostProcessQuality(int PostProcessQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.PostProcessQuality %d"), PostProcessQuality));
}

void UPerformanceLibrary::SetShadowQuality(int ShadowQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ShadowQuality %d"), ShadowQuality));
}

void UPerformanceLibrary::SetGlobalIlluminationQuality(int GlobalIlluminationQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.GlobalIlluminationQuality %d"), GlobalIlluminationQuality));
}

void UPerformanceLibrary::SetReflectionQuality(int ReflectionQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ReflectionQuality %d"), ReflectionQuality));
}

void UPerformanceLibrary::SetTextureQuality(int TextureQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.TextureQuality %d"), TextureQuality));
}

void UPerformanceLibrary::SetEffectsQuality(int EffectsQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.EffectsQuality %d"), EffectsQuality));
}

void UPerformanceLibrary::SetFoliageQuality(int FoliageQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.FoliageQuality %d"), FoliageQuality));
}

void UPerformanceLibrary::SetShadingQuality(int ShadingQuality)
{
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ShadingQuality %d"), ShadingQuality));
}

FScalabilityInfo UPerformanceLibrary::GetScalability()
{
	FScalabilityInfo ScalabilityInfo;
	ScalabilityInfo.ResolutionScale = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.ResolutionQuality");
	ScalabilityInfo.ViewDistance    = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.ViewDistanceQuality");
	ScalabilityInfo.AntiAliasing = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.AntiAliasingQuality");
	ScalabilityInfo.PostProcess = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.PostProcessQuality");
	ScalabilityInfo.Shadow = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.ShadowQuality");
	ScalabilityInfo.GlobalIllumination = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.GlobalIlluminationQuality");
	ScalabilityInfo.Reflection = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.ReflectionQuality");
	ScalabilityInfo.Texture = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.TextureQuality");
	ScalabilityInfo.Effects = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.EffectsQuality");
	ScalabilityInfo.Foliage = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.FoliageQuality");
	ScalabilityInfo.Shading = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.ShadingQuality");
	return ScalabilityInfo;
}



void UPerformanceLibrary::StartTrace(const FName Name)
{
#if CPUPROFILERTRACE_ENABLED
	if (UE_TRACE_CHANNELEXPR_IS_ENABLED(CpuChannel))
	{
		FCpuProfilerTrace::OutputBeginDynamicEvent(Name);
	}
#endif
}

void UPerformanceLibrary::StopTrace()
{
#if CPUPROFILERTRACE_ENABLED
		FCpuProfilerTrace::OutputEndEvent();
#endif
}