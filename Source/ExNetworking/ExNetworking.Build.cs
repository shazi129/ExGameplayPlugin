using UnrealBuildTool;

public class ExNetworking : ModuleRules
{
    public ExNetworking(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "Core",
            "CoreUObject", 
            "Engine",
            "Sockets",
            "Networking"
        });
    }
}