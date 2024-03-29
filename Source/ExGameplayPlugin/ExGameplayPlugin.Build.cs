// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ExGameplayPlugin : ModuleRules
{
	public ExGameplayPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Public") 
				}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Engine",
				"Core",
				"EnhancedInput",
				"ModularGameplay",
				"Niagara",
				"GameplayTags",
				"DeveloperSettings",
				"ExGameplayLibrary",
				"ExInputSystem",
				"GameplayAbilities",
                "ExGameplayAbilities",
                "GameFeatures",
				"Json",
				"HTTP",
				"AnimToTexture",
                "GameplayUtils"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
