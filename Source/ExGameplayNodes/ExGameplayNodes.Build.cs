// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class ExGameplayNodes : ModuleRules
	{
        public ExGameplayNodes(ReadOnlyTargetRules Target) : base(Target)
		{
			//OverridePackageType = PackageOverrideType.EngineDeveloper;
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);

            PublicIncludePaths.AddRange(
	            new string[] {
					// ... add public include paths required here ...
					Path.Combine(ModuleDirectory, "Public"),
				}
			);

            PublicDependencyModuleNames.AddRange(
				new string[] 
				{
					"Core",
					"CoreUObject",
					"Engine",
					"BlueprintGraph",
					"GameplayAbilitiesEditor",
					"GameplayTasksEditor"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[] 
				{
					"AssetTools",
					"ClassViewer",
					"GameplayTags",
					"GameplayTagsEditor",
					"GameplayAbilities",
					"GameplayTasksEditor",
					"InputCore",
					"PropertyEditor",
					"Slate",
					"SlateCore",
					"EditorStyle",
					"BlueprintGraph",
					"Kismet",
					"KismetCompiler",
					"GraphEditor",
					"MainFrame",
					"UnrealEd",
					"WorkspaceMenuStructure",
					"ContentBrowser",
					"EditorWidgets",
					"SourceControl",
					
				}
			);
		}
	}
}
