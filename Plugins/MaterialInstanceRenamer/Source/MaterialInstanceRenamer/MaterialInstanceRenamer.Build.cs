// Copyright 2025 kurorekish. All Rights Reserved.

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
                "SlateCore",
                "Settings"
            }
        );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
            PrivateDependencyModuleNames.Add("EditorStyle");
        }
    }
}