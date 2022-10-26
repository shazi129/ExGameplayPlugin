#include "PerformanceLibrary.h"
#include "ExGameplayLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.EffectsQuality %d"), ScalabilityInfo.Effect));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.FoliageQuality %d"), ScalabilityInfo.Foliage));
	UExGameplayLibrary::ExecCommand(FString::Printf(TEXT("sg.ShadingQuality %d"), ScalabilityInfo.Shading));
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
	ScalabilityInfo.Effect = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.EffectsQuality");
	ScalabilityInfo.Foliage = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.FoliageQuality");
	ScalabilityInfo.Shading = UKismetSystemLibrary::GetConsoleVariableIntValue("sg.ShadingQuality");
	return ScalabilityInfo;
}