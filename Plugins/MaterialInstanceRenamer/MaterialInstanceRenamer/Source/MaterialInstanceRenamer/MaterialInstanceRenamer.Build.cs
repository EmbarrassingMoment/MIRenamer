// MaterialInstanceRenamer.Build.cs

using UnrealBuildTool;

public class MaterialInstanceRenamer : ModuleRules
{
    public MaterialInstanceRenamer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "AssetTools",
                "AssetRegistry",
                "ContentBrowser",
                "ToolMenus"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore"
            }
        );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
            PrivateDependencyModuleNames.Add("EditorStyle");
        }
    }
}