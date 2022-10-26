#include "PerformanceLibrary.h"
#include "ExGameplayLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UPerformanceLibrary::SetScalability(FScalabilityInfo ScalabilityInfo)
{
	UExGameplayLibrary::ExecCommand("sg.ResolutionQuality " + ScalabilityInfo.ResolutionScale);
	UExGameplayLibrary::ExecCommand("sg.ViewDistanceQuality " + ScalabilityInfo.ViewDistance);
	UExGameplayLibrary::ExecCommand("sg.AntiAliasingQuality " + ScalabilityInfo.AntiAliasing);
	UExGameplayLibrary::ExecCommand("sg.PostProcessQuality " + ScalabilityInfo.PostProcess);
	UExGameplayLibrary::ExecCommand("sg.ShadowQuality " + ScalabilityInfo.Shadow);
	UExGameplayLibrary::ExecCommand("sg.GlobalIlluminationQuality " + ScalabilityInfo.GlobalIllumination);
	UExGameplayLibrary::ExecCommand("sg.ReflectionQuality " + ScalabilityInfo.Reflection);
	UExGameplayLibrary::ExecCommand("sg.TextureQuality " + ScalabilityInfo.Texture);
	UExGameplayLibrary::ExecCommand("sg.EffectsQuality " + ScalabilityInfo.Effect);
	UExGameplayLibrary::ExecCommand("sg.FoliageQuality " + ScalabilityInfo.Foliage);
	UExGameplayLibrary::ExecCommand("sg.ShadingQuality " + ScalabilityInfo.Shading);
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